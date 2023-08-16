#include "3tle3wa/pass/interprocedural/peephole/peephole.hh"

#include <cassert>
#include <cstdint>
#include <iterator>
#include <memory>
#include <unordered_map>

#include "3tle3wa/ir/function/basicblock.hh"
#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/instruction/binaryOpInst.hh"
#include "3tle3wa/ir/instruction/compareInst.hh"
#include "3tle3wa/ir/instruction/controlFlowInst.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/instruction/memoryInst.hh"
#include "3tle3wa/ir/instruction/opCode.hh"
#include "3tle3wa/ir/instruction/otherInst.hh"
#include "3tle3wa/ir/instruction/unaryOpInst.hh"
#include "3tle3wa/ir/value/baseValue.hh"
#include "3tle3wa/ir/value/constant.hh"
#include "3tle3wa/ir/value/constarray.hh"
#include "3tle3wa/ir/value/globalvalue.hh"
#include "3tle3wa/ir/value/type/scalarType.hh"
#include "3tle3wa/ir/value/variable.hh"

void PeepHole::PeepHoleOpt(NormalFuncPtr func) {
    for (auto &&node : func->GetSequentialNodes()) {
        auto &&inst_list = node->GetInstList();
        for (auto &&iter = inst_list.begin(); iter != inst_list.end();) {
            auto &&inst = (*iter);
            if (inst->IsPhiInst()) {  // fabs
                auto &&phi = std::static_pointer_cast<PhiInst>(inst);
                auto &&phi_result = phi->GetResult();

                auto &&phi_list = phi->GetDataList();
                if (phi_list.size() == 2) {
                    auto &&[value1, block1] = phi_list.front();
                    auto &&[value2, block2] = phi_list.back();
                    // check simple if-else
                    if (block1->GetPredecessors().size() == 1 && block2->GetPredecessors().size() == 1 &&
                        (*block1->GetPredecessors().begin() == *block2->GetPredecessors().begin())) {
                        auto &&cond_blk = (*block1->GetPredecessors().begin());
                        // cout << cond_blk->tollvmIR() << endl;

                        if (auto &&br_inst = std::dynamic_pointer_cast<BranchInst>(cond_blk->GetLastInst())) {
                            auto &&cond = br_inst->GetCondition();
                            auto &&iftrue = br_inst->GetTrueTarget();
                            auto &&iffalse = br_inst->GetFalseTarget();
                            // check float compare
                            if (auto &&fcmp_inst = std::dynamic_pointer_cast<FCmpInst>(cond->GetParent())) {
                                auto opcode = fcmp_inst->GetOpCode();
                                auto &&lhs = fcmp_inst->GetLHS();
                                auto &&rhs = fcmp_inst->GetRHS();
                                auto &&zero = ConstantAllocator::FindConstantPtr(static_cast<float>(0));

                                std::unordered_map<CtrlFlowGraphNode *, BaseValuePtr> map;
                                map[block1.get()] = value1;
                                map[block2.get()] = value2;
                                // check fabs condition
                                if ((opcode == OP_LTH || opcode == OP_LEQ) && rhs == zero) {
                                    if (lhs == map[iffalse.get()]) {
                                        if (auto &&fneg_inst =
                                                std::dynamic_pointer_cast<FNegInst>(map[iftrue.get()]->GetParent());
                                            fneg_inst != nullptr && fneg_inst->GetOprand() == lhs) {
                                            // create fabs-inst
                                            auto &&fabs_inst = FAbsInst::CreatePtr(phi_result, lhs, cond_blk);
                                            lhs->InsertUser(fabs_inst);
                                            phi_result->SetParent(fabs_inst);
                                            // insert fabs-inst into cond-blk
                                            auto &&cond_blk_inst_list = cond_blk->GetInstList();
                                            auto &&cond_blk_iter = cond_blk_inst_list.end();
                                            std::advance(cond_blk_iter, -1);
                                            cond_blk_inst_list.insert(cond_blk_iter, fabs_inst);
                                            // simpfly
                                            if (iftrue->GetInstCnt() == 2 && iffalse->GetInstCnt() == 1) {
                                                RemoveInst(br_inst);
                                                cond_blk->RemoveLastInst();
                                                RemoveNode(iftrue);
                                                RemoveNode(iffalse);
                                                cond_blk->InsertInstBack(JumpInst::CreatePtr(node, cond_blk));
                                            }
                                            // remove ori-phi-inst
                                            iter = inst_list.erase(iter);
                                            continue;
                                        }
                                    }
                                } else if ((opcode == OP_GTH || opcode == OP_GEQ) && rhs == zero) {
                                    if (lhs == map[iftrue.get()]) {
                                        if (auto &&fneg_inst =
                                                std::dynamic_pointer_cast<FNegInst>(map[iffalse.get()]->GetParent());
                                            fneg_inst != nullptr && fneg_inst->GetOprand() == lhs) {
                                            // create fabs-inst
                                            auto &&fabs_inst = FAbsInst::CreatePtr(phi_result, lhs, cond_blk);
                                            lhs->InsertUser(fabs_inst);
                                            phi_result->SetParent(fabs_inst);
                                            // insert fabs-inst into cond-blk
                                            auto &&cond_blk_inst_list = cond_blk->GetInstList();
                                            auto &&cond_blk_iter = cond_blk_inst_list.end();
                                            std::advance(cond_blk_iter, -1);
                                            cond_blk_inst_list.insert(cond_blk_iter, fabs_inst);
                                            // simpfly
                                            if (iftrue->GetInstCnt() == 1 && iffalse->GetInstCnt() == 2) {
                                                RemoveInst(br_inst);
                                                cond_blk->RemoveLastInst();
                                                RemoveNode(iftrue);
                                                RemoveNode(iffalse);
                                                cond_blk->InsertInstBack(JumpInst::CreatePtr(node, cond_blk));
                                            }
                                            // remove ori-phi-inst
                                            iter = inst_list.erase(iter);
                                            continue;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (inst->IsGepInst()) {  // remove redundant gep-inst
                auto &&gep2 = std::static_pointer_cast<GetElementPtrInst>(inst);
                auto &&base_addr2 = gep2->GetBaseAddr();
                auto &&off_list2 = gep2->GetOffList();
                if (off_list2.size() == 1 && base_addr2->GetParent() && base_addr2->GetParent()->IsGepInst()) {
                    auto &&gep1 = std::static_pointer_cast<GetElementPtrInst>(base_addr2->GetParent());
                    auto &&base_addr1 = gep1->GetBaseAddr();
                    auto &&off_list1 = gep1->GetOffList();
                    if (off_list1.size() == 2) {
                        auto &&real_off1 = off_list1.back();
                        auto &&real_off2 = off_list2.back();

                        if (real_off1->IsConstant()) {
                            auto &&zero = ConstantAllocator::FindConstantPtr(static_cast<int32_t>(0));

                            auto &&constant1 = std::static_pointer_cast<Constant>(real_off1);
                            if (real_off1 == zero) {
                                // replace base-addr
                                gep2->SetBaseAddr(base_addr1);
                                base_addr2->RemoveUser(gep2);
                                base_addr1->InsertUser(gep2);
                                base_addr1->RemoveUser(gep1);
                                // no need to change gep2->off_list
                            } else {
                                if (real_off2->IsConstant()) {
                                    auto &&constant2 = std::static_pointer_cast<Constant>(real_off2);
                                    // replace base-addr
                                    gep2->SetBaseAddr(base_addr1);
                                    base_addr2->RemoveUser(gep2);
                                    base_addr1->InsertUser(gep2);
                                    base_addr1->RemoveUser(gep1);
                                    // calculate new offset
                                    int32_t off_int1 = std::get<int32_t>(constant1->GetValue());
                                    int32_t off_int2 = std::get<int32_t>(constant2->GetValue());
                                    auto &&new_real_off2 =
                                        ConstantAllocator::FindConstantPtr(static_cast<int32_t>(off_int1 + off_int2));
                                    // set new offset-list
                                    auto &&off_list = OffsetList(1, new_real_off2);
                                    gep2->SetOffList(off_list);
                                }
                            }
                        }
                    }
                }
            } else if (inst->IsLoadInst()) {
                auto &&load_inst = std::static_pointer_cast<LoadInst>(inst);
                if (load_inst->GetOprand()->IsVariable()) {  // load-addr, variable or global-value
                    auto &&load_addr = std::static_pointer_cast<Variable>(load_inst->GetOprand());
                    if (load_addr->GetParent() && load_addr->GetParent()->IsGepInst()) {
                        auto &&gep_inst = std::static_pointer_cast<GetElementPtrInst>(load_addr->GetParent());

                        auto &&base_addr = gep_inst->GetBaseAddr();
                        auto &&offset = gep_inst->GetOffList().back();
                        if (base_addr->IsGlobalValue() && offset->IsConstant()) {
                            auto &&glb_value = std::static_pointer_cast<GlobalValue>(base_addr);

                            auto &&init_value = glb_value->GetInitValue();
                            if (init_value->GetBaseType()->IsImMutable() && init_value->IsConstArray()) {
                                const auto &&const_arr = std::static_pointer_cast<ConstArray>(init_value);
                                auto &&init_arr = const_arr->GetConstArr();
                                auto &&load_value =
                                    init_arr[std::get<int32_t>(std::static_pointer_cast<Constant>(offset)->GetValue())];
                                ReplaceSRC(load_inst->GetResult(), load_value);
                            }
                        }
                    }
                }
            }
            ++iter;
        }
    }
}