#include "3tle3wa/pass/interprocedural/rec2loop/rec2loop.hh"

#include <cassert>
#include <unordered_map>

#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/function/structure/loop.hh"
#include "3tle3wa/ir/instruction/compareInst.hh"
#include "3tle3wa/ir/instruction/controlFlowInst.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/instruction/otherInst.hh"
#include "3tle3wa/ir/value/baseValue.hh"
#include "3tle3wa/ir/value/constant.hh"

void TailRec2Loop::DoTailRec2Loop(NormalFuncPtr &func, SymbolTable &glb_table) {
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

void TailRec2Loop::TailRec2Loop(NormalFuncPtr &func) {
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

            auto new_entry = func->CreateCfgNode(BlkAttr::Entry | BlkAttr::Normal);  // new entry
            auto loop_cond = func->CreateCfgNode(BlkAttr::Normal);                   // loop cond
            auto loop_body = func->CreateCfgNode(BlkAttr::Normal);                   // loop body
            auto new_exit = func->CreateCfgNode(BlkAttr::Exit | BlkAttr::Normal);    // new exit

            // rebuild entry
            auto &&fparam_list = func->GetParamList();
            for (auto &&fparam : fparam_list) {
                auto &&phi_inst = PhiInst::CreatePtr(fparam->GetBaseType(), new_entry);
                PhiInst::InsertPhiData(phi_inst, fparam, loop_cond);

                value_map[fparam.get()] = phi_inst->GetResult();
            }
            new_entry->InsertInstBack(JumpInst::CreatePtr(loop_cond, new_entry));
            // copy branch cond
            // copy cmp_inst
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
            // copy br_inst
            loop_cond->InsertInstBack(BranchInst::CreatePtr(result, loop_body, new_exit, loop_cond));

            // if (map[iftrue]->GetParent() && map[iftrue]->GetParent()->IsCallInst()) {
            // } else if (map[iffalse]->GetParent() && map[iffalse]->GetParent()->IsCallInst()) {
            // }

            // for (auto &&node : func->GetSequentialNodes()) {
            //     RemoveNode(node);
            // }
            // func->SetEntryNode(new_entry);
            // func->SetExitNode(new_exit);
        }
    }
    value_map.clear();
}

BaseValuePtr TailRec2Loop::InstCopy(InstPtr &inst, CfgNodePtr &node) {
    BaseValuePtr result = nullptr;
    return result;
}