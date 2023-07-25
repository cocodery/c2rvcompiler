#include "3tle3wa/pass/interprocedural/loop/loopInvariant.hh"

void LoopInvariant::LoopInvariant(NormalFuncPtr func) {
    // PrintLoop(*(func->loops));

    InvariantMotion(func->loops);
}

void LoopInvariant::InvariantMotion(Loop *loop) {
    for (auto &&sub_loop : loop->sub_loops) {
        InvariantMotion(sub_loop);
    }
    if (loop->before_loop) {
        InvariantsInBlocks invariablesInBlocks = FindInvariant(loop);
        for (auto &&blkAndInvariants : invariablesInBlocks) {
            auto &&nowBlk = blkAndInvariants.first;
            auto &&newBlk = loop->before_loop;
            for (auto &&invariant : blkAndInvariants.second) {
                auto &&inst = invariant->GetParent();
                auto &&instList = newBlk->GetInstList();
                auto &&end = instList.end();
                std::advance(end, -1);
                instList.insert(end, inst);
                nowBlk->RemoveInst(inst);
                inst->SetParent(std::make_shared<CtrlFlowGraphNode>(*newBlk));
            }
        }
    }
}

InvariantsInBlocks LoopInvariant::FindInvariant(Loop *loop) {
    LoopBlocks &&loop_blks = loop->GetEntireLoop();

    std::set<BaseValuePtr> defined_in_loop;
    std::set<VariablePtr> invariant;
    InvariantsInBlocks invariants;
    for (auto &&blk : loop_blks) {
        for (auto &&inst : blk->GetInstList()) {
            if (inst->IsStoreInst()) {
                auto &&binary_inst = std::static_pointer_cast<BinaryInstruction>(inst);
                defined_in_loop.insert(binary_inst->GetLHS());
            } else if (!inst->IsCriticalOperation()) {
                defined_in_loop.insert(inst->GetResult());
            }
        }
    }

    bool change = false;
    do {
        change = false;
        for (auto &&blk : loop_blks) {
            invariant.clear();
            for (auto &&inst : blk->GetInstList()) {
                bool invariant_check = true;
                if (inst->IsCallInst() || inst->IsJumpInst() || inst->IsReturnInst() || inst->IsBranchInst() ||
                    inst->IsPhiInst()) {
                    continue;
                }
                if (inst->IsStoreInst()) {
                    auto &&binary_inst = std::static_pointer_cast<BinaryInstruction>(inst);

                    auto &&lhs = binary_inst->GetLHS();
                    auto &&rhs = binary_inst->GetRHS();

                    if (defined_in_loop.find(lhs) == defined_in_loop.end()) {
                        continue;
                    }

                    if (defined_in_loop.find(rhs) != defined_in_loop.end()) {
                        invariant_check = false;
                    }
                } else {
                    if (defined_in_loop.find(inst->GetResult()) == defined_in_loop.end()) {
                        continue;
                    }
                    if (inst->IsOneOprandInst()) {
                        auto &&unary_inst = std::static_pointer_cast<UnaryInstruction>(inst);

                        auto &&oprand = unary_inst->GetOprand();
                        if (defined_in_loop.find(oprand) != defined_in_loop.end()) {
                            invariant_check = false;
                        }
                    } else if (inst->IsTwoOprandInst()) {
                        auto &&binary_inst = std::static_pointer_cast<BinaryInstruction>(inst);

                        auto &&lhs = binary_inst->GetLHS();
                        auto &&rhs = binary_inst->GetRHS();

                        if (defined_in_loop.find(lhs) != defined_in_loop.end() ||
                            defined_in_loop.find(rhs) != defined_in_loop.end()) {
                            invariant_check = false;
                        }
                    }
                }
                if (invariant_check) {
                    if (inst->IsStoreInst()) {
                        auto &&binary_inst = std::static_pointer_cast<BinaryInstruction>(inst);

                        auto &&lhs = binary_inst->GetLHS();
                        defined_in_loop.erase(lhs);
                        invariant.insert(std::static_pointer_cast<Variable>(lhs));
                    } else {
                        defined_in_loop.erase(inst->GetResult());
                        invariant.insert(inst->GetResult());
                    }
                    change = true;
                }
            }
            if (!invariant.empty()) {
                auto variable = *invariant.begin();
                auto blkPtr = variable->GetParent()->GetParent();
                invariants.push_back({blkPtr, invariant});
            }
        }
    } while (change);
    return invariants;
}
