#include "3tle3wa/pass/interprocedural/recopt/recopt.hh"

#include <cassert>
#include <unordered_map>

#include "3tle3wa/ir/function/basicblock.hh"
#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/function/structure/loop.hh"
#include "3tle3wa/ir/instruction/compareInst.hh"
#include "3tle3wa/ir/instruction/controlFlowInst.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/instruction/memoryInst.hh"
#include "3tle3wa/ir/instruction/opCode.hh"
#include "3tle3wa/ir/instruction/otherInst.hh"
#include "3tle3wa/ir/value/baseValue.hh"
#include "3tle3wa/ir/value/constant.hh"

void RecursionOpt::DoTailRec2Loop(NormalFuncPtr &func, SymbolTable &glb_table) {
    assert(value_map.size() == 0);
    for (auto &&[_, value] : glb_table.GetNameValueMap()) {
        value_map[value.get()] = value;
    }
    for (auto &&[_, constant] : ConstantAllocator::GetConstantAllocator()) {
        value_map[constant.get()] = constant;
    }
    TailRec2Loop(func);
    value_map.clear();
}

void RecursionOpt::TailRec2Loop(NormalFuncPtr &func) {
    // check recursion
    if (func->GetRecursive() == false) return;
    // check no pointer parameter
    for (auto &&param : func->GetParamList()) {
        if (param->GetBaseType()->IsPointer()) {
            return;
        }
    }
    // check tail-recursion
    size_t tail_recursion_cnt = 0;
    std::list<CallInst *> tail_recursion_insts;
    auto &&seq_nodes = func->GetSequentialNodes();
    for (auto &&node : seq_nodes) {
        for (auto &&inst : node->GetInstList()) {
            if (inst->IsCallInst()) {
                auto call_inst = static_cast<CallInst *>(inst.get());

                if (call_inst->GetTailCall() && func.get() == call_inst->GetCalleeFunc().get()) {
                    tail_recursion_cnt += 1;
                    tail_recursion_insts.push_back(call_inst);
                }
            }
        }
    }
    if (tail_recursion_cnt != 1) return;

    auto ChkSimpleTailRecCall = [&seq_nodes]() -> bool {
        if (seq_nodes.size() != 4) return false;
        if (seq_nodes.front()->GetInstCnt() > 2) return false;

        auto ret_value = static_cast<ReturnInst *>(seq_nodes.back().get()->GetLastInst().get())->GetRetValue();
        if (ret_value->GetParent()->IsPhiInst() == false) return false;
        auto phi = static_cast<PhiInst *>(ret_value->GetParent().get());

        auto &&data_list = phi->GetDataList();
        if (data_list.size() > 2) return false;
        auto &&value1 = data_list.front().first;
        auto &&value2 = data_list.back().first;
        if (value1->GetParent() && value1->GetParent()->IsCallInst()) {
            auto call_inst = static_cast<CallInst *>(value1->GetParent().get());
            if (!call_inst->GetTailCall()) return false;
        }
        if (value2->GetParent() && value2->GetParent()->IsCallInst()) {
            auto call_inst = static_cast<CallInst *>(value2->GetParent().get());
            if (!call_inst->GetTailCall()) return false;
        }
        return true;
    };

    if (func->GetReturnType()->VoidType() == false) {  // non-void
        // Process Simple Tail-Recursion-Call first
        if (ChkSimpleTailRecCall()) {
            auto entry = func->GetEntryNode().get();  // branch cond
            auto exit = func->GetExitNode().get();

            assert(entry->GetInstCnt() == 2);
            auto cmp_inst = dynamic_cast<BinaryInstruction *>(entry->GetInstList().front().get());
            auto br_inst = dynamic_cast<BranchInst *>(entry->GetInstList().back().get());
            assert(cmp_inst && br_inst);

            auto iftrue = br_inst->GetTrueTarget().get();
            auto iffalse = br_inst->GetFalseTarget().get();

            auto ret_inst = static_cast<ReturnInst *>(exit->GetLastInst().get());
            assert(ret_inst);
            auto phi_inst = static_cast<PhiInst *>(ret_inst->GetRetValue()->GetParent().get());
            assert(phi_inst);

            std::unordered_map<CtrlFlowGraphNode *, BaseValuePtr> map;
            for (auto &&[value, block] : phi_inst->GetDataList()) {
                map[block.get()] = value;
            }

            auto new_entry = func->CreateCfgNode(BlkAttr::Entry | BlkAttr::Normal);     // new entry
            auto loop_cond = func->CreateCfgNode(BlkAttr::Normal);                      // loop cond
            auto loop_body = func->CreateCfgNode(BlkAttr::Normal | BlkAttr::Continue);  // loop body
            auto new_exit = func->CreateCfgNode(BlkAttr::Exit | BlkAttr::Normal);       // new exit

            // copy entry
            // wait for insert jump
            // copy cond-begin
            auto &&fparam_list = func->GetParamList();
            std::unordered_map<BaseValue *, PhiInstPtr> phi_map;
            for (auto &&fparam : fparam_list) {
                auto &&phi_inst = PhiInst::CreatePtr(fparam->GetBaseType(), loop_cond);
                PhiInst::InsertPhiData(phi_inst, fparam, new_entry);

                value_map[fparam.get()] = phi_inst->GetResult();
                phi_map[fparam.get()] = phi_inst;
            }
            auto opcode = cmp_inst->GetOpCode();
            auto lhs = cmp_inst->GetLHS().get();
            auto rhs = cmp_inst->GetRHS().get();
            BaseValuePtr result = nullptr;
            if (cmp_inst->IsICmpInst()) {
                result = ICmpInst::DoICompare(opcode, value_map[lhs], value_map[rhs], loop_cond);
            } else if (cmp_inst->IsFCmpInst()) {
                result = FCmpInst::DoFCompare(opcode, value_map[lhs], value_map[rhs], loop_cond);
            }
            assert(result != nullptr);
            value_map[cmp_inst->GetResult().get()] = result;
            if (map[iftrue]->GetParent() && map[iftrue]->GetParent()->IsCallInst()) {
                // insert branch
                loop_cond->InsertInstBack(BranchInst::CreatePtr(result, loop_body, new_exit, loop_cond));
                // copy loop-body
                for (auto &&inst : iftrue->GetInstList()) {
                    if (inst->IsBranchInst() || inst->IsJumpInst() || inst->IsReturnInst()) {
                        // do nothing
                    } else if (inst->IsAllocaInst()) {
                        auto alloca = static_cast<AllocaInst *>(inst.get());
                        value_map[inst->GetResult().get()] = AllocaInst::DoAllocaAddr(
                            alloca->GetAllocaType(), alloca->GetAllocaAddr()->GetBaseType(), new_entry);
                    } else if (inst->IsStoreInst()) {
                        auto &&store = static_cast<StoreInst *>(inst.get());
                        StoreInst::DoStoreValue(value_map[store->GetStoreAddr().get()],
                                                value_map[store->GetStoreValue().get()], loop_body);
                    } else if (inst->IsCallInst()) {
                        auto call = static_cast<CallInst *>(inst.get());
                        if (call->GetTailCall() == false) {
                            ParamList param_list;
                            for (auto &&param : call->GetParamList()) {
                                param_list.push_back(value_map[param.get()]);
                            }
                            value_map[call->GetResult().get()] = CallInst::DoCallFunction(
                                call->GetRetType(), call->GetCalleeFunc(), param_list, loop_body);
                        } else {
                            auto &&rparam_list = call->GetParamList();
                            for (auto &&f_iter = fparam_list.begin(), r_iter = rparam_list.begin();
                                 f_iter != fparam_list.end(); ++f_iter, ++r_iter) {
                                auto &&phi_inst = phi_map[(*f_iter).get()];
                                PhiInst::InsertPhiData(phi_inst, value_map[(*r_iter).get()], loop_body);
                            }
                        }
                    } else {
                        auto &&result = InstCopy(inst.get(), loop_body);
                        value_map[inst->GetResult().get()] = result;
                    }
                }
                loop_body->InsertInstBack(JumpInst::CreatePtr(loop_cond, loop_body));
                // later insert jump, avoid alloca in loop
                new_entry->InsertInstBack(JumpInst::CreatePtr(loop_cond, new_entry));
                // copy exit
                for (auto &&inst : iffalse->GetInstList()) {
                    if (inst->IsBranchInst() || inst->IsJumpInst() || inst->IsReturnInst()) {
                        // do nothing
                    } else if (inst->IsAllocaInst()) {
                        auto alloca = static_cast<AllocaInst *>(inst.get());
                        value_map[inst->GetResult().get()] = AllocaInst::DoAllocaAddr(
                            alloca->GetAllocaType(), alloca->GetAllocaAddr()->GetBaseType(), new_entry);
                    } else if (inst->IsStoreInst()) {
                        auto &&store = static_cast<StoreInst *>(inst.get());
                        StoreInst::DoStoreValue(value_map[store->GetStoreAddr().get()],
                                                value_map[store->GetStoreValue().get()], new_exit);
                    } else if (inst->IsCallInst()) {
                        auto call = static_cast<CallInst *>(inst.get());
                        if (call->GetTailCall() == false) {
                            ParamList param_list;
                            for (auto &&param : call->GetParamList()) {
                                param_list.push_back(value_map[param.get()]);
                            }
                            value_map[call->GetResult().get()] = CallInst::DoCallFunction(
                                call->GetRetType(), call->GetCalleeFunc(), param_list, new_exit);
                        } else {
                            assert(false);
                        }
                    } else {
                        value_map[inst->GetResult().get()] = InstCopy(inst.get(), new_exit);
                    }
                }
                auto &&ret_value = value_map[map[iffalse].get()];
                auto ret = ReturnInst::CreatePtr(func->GetReturnType(), ret_value, new_exit);
                ret_value->InsertUser(ret);
                new_exit->InsertInstBack(ret);
            } else if (map[iffalse]->GetParent() && map[iffalse]->GetParent()->IsCallInst()) {
                // insert branch
                loop_cond->InsertInstBack(BranchInst::CreatePtr(result, new_exit, loop_body, loop_cond));
                // copy loop-body
                for (auto &&inst : iffalse->GetInstList()) {
                    if (inst->IsBranchInst() || inst->IsJumpInst() || inst->IsReturnInst()) {
                        // do nothing
                    } else if (inst->IsAllocaInst()) {
                        auto alloca = static_cast<AllocaInst *>(inst.get());
                        value_map[inst->GetResult().get()] = AllocaInst::DoAllocaAddr(
                            alloca->GetAllocaType(), alloca->GetAllocaAddr()->GetBaseType(), new_entry);
                    } else if (inst->IsStoreInst()) {
                        auto &&store = static_cast<StoreInst *>(inst.get());
                        StoreInst::DoStoreValue(value_map[store->GetStoreAddr().get()],
                                                value_map[store->GetStoreValue().get()], loop_body);
                    } else if (inst->IsCallInst()) {
                        auto call = static_cast<CallInst *>(inst.get());
                        if (call->GetTailCall() == false) {
                            ParamList param_list;
                            for (auto &&param : call->GetParamList()) {
                                param_list.push_back(value_map[param.get()]);
                            }
                            value_map[call->GetResult().get()] = CallInst::DoCallFunction(
                                call->GetRetType(), call->GetCalleeFunc(), param_list, loop_body);
                        } else {
                            auto &&rparam_list = call->GetParamList();
                            for (auto &&f_iter = fparam_list.begin(), r_iter = rparam_list.begin();
                                 f_iter != fparam_list.end(); ++f_iter, ++r_iter) {
                                auto &&phi_inst = phi_map[(*f_iter).get()];
                                PhiInst::InsertPhiData(phi_inst, value_map[(*r_iter).get()], loop_body);
                            }
                        }
                    } else {
                        auto &&result = InstCopy(inst.get(), loop_body);
                        value_map[inst->GetResult().get()] = result;
                    }
                }
                loop_body->InsertInstBack(JumpInst::CreatePtr(loop_cond, loop_body));
                // later insert jump, avoid alloca in loop
                new_entry->InsertInstBack(JumpInst::CreatePtr(loop_cond, new_entry));
                // copy exit
                for (auto &&inst : iftrue->GetInstList()) {
                    if (inst->IsBranchInst() || inst->IsJumpInst() || inst->IsReturnInst()) {
                        // do nothing
                    } else if (inst->IsAllocaInst()) {
                        auto alloca = static_cast<AllocaInst *>(inst.get());
                        value_map[inst->GetResult().get()] = AllocaInst::DoAllocaAddr(
                            alloca->GetAllocaType(), alloca->GetAllocaAddr()->GetBaseType(), new_exit);
                    } else if (inst->IsStoreInst()) {
                        auto &&store = static_cast<StoreInst *>(inst.get());
                        StoreInst::DoStoreValue(value_map[store->GetStoreAddr().get()],
                                                value_map[store->GetStoreValue().get()], new_exit);
                    } else if (inst->IsCallInst()) {
                        auto call = static_cast<CallInst *>(inst.get());
                        if (call->GetTailCall() == false) {
                            ParamList param_list;
                            for (auto &&param : call->GetParamList()) {
                                param_list.push_back(value_map[param.get()]);
                            }
                            value_map[call->GetResult().get()] = CallInst::DoCallFunction(
                                call->GetRetType(), call->GetCalleeFunc(), param_list, new_exit);
                        } else {
                            assert(false);
                        }
                    } else {
                        auto &&result = InstCopy(inst.get(), new_exit);
                        value_map[inst->GetResult().get()] = result;
                    }
                }
                auto &&ret_value = value_map[map[iftrue].get()];
                auto ret = ReturnInst::CreatePtr(func->GetReturnType(), ret_value, new_exit);
                ret_value->InsertUser(ret);
                new_exit->InsertInstBack(ret);
            } else {
                assert(false);
            }
            for (auto &&node : func->GetSequentialNodes()) {
                RemoveNode(node);
            }
            func->SetEntryNode(new_entry);
            func->SetExitNode(new_exit);
        }
    }
    value_map.clear();
}

BaseValuePtr RecursionOpt::InstCopy(Instruction *inst, CfgNodePtr &node) {
    BaseValuePtr result = nullptr;
    auto opcode = inst->GetOpCode();
    if (inst->IsOneOprandInst()) {
        auto unary_inst = static_cast<UnaryInstruction *>(inst);
        auto &&oprand = value_map[unary_inst->GetOprand().get()];
        if (opcode == Load) {
            result = LoadInst::DoLoadValue(oprand, node);
        } else if (opcode == BitCast) {
            result = BitCastInst::DoBitCast(oprand, node);
        } else if (opcode == SiToFp) {
            result = SitoFpInst::DoSitoFp(oprand, node);
        } else if (opcode == FpToSi) {
            auto type = unary_inst->GetResult()->GetBaseType()->GetAttrType();
            result = FptoSiInst::DoFptoSi(type, oprand, node);
        } else if (opcode == Zext) {
            result = ZextInst::DoZeroExt(oprand, node);
        } else if (opcode == FNeg) {
            result = FNegInst::DoFloatNeg(oprand, node);
        } else {
            assert(false);
        }
    } else if (inst->IsTwoOprandInst()) {
        auto binary_inst = static_cast<BinaryInstruction *>(inst);
        auto &&lhs = value_map[binary_inst->GetLHS().get()];
        auto &&rhs = value_map[binary_inst->GetRHS().get()];
        if (binary_inst->IsIBinaryInst()) {
            result = IBinaryInst::DoIBinOperate(opcode, lhs, rhs, node);
        } else if (binary_inst->IsFBinaryInst()) {
            result = FBinaryInst::DoFBinOperate(opcode, lhs, rhs, node);
        } else if (binary_inst->IsICmpInst()) {
            result = ICmpInst::DoICompare(opcode, lhs, rhs, node);
        } else if (binary_inst->IsFCmpInst()) {
            result = FCmpInst::DoFCompare(opcode, lhs, rhs, node);
        } else {
            assert(false);
        }
    } else if (inst->IsGepInst()) {
        auto gep_inst = static_cast<GetElementPtrInst *>(inst);
        OffsetList off_list;
        for (auto &&off : gep_inst->GetOffList()) {
            off_list.push_back(value_map[off.get()]);
        }
        result = GetElementPtrInst::DoGetPointer(gep_inst->GetStoreType(), value_map[gep_inst->GetBaseAddr().get()],
                                                 off_list, node);
    }
    assert(result != nullptr);
    return result;
}