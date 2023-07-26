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
                nowBlk->RemoveInst(invariant);
                auto &&instList = newBlk->GetInstList();
                auto &&end = instList.end();
                std::advance(end, -1);
                instList.insert(end, invariant);
                invariant->SetParent(std::make_shared<CtrlFlowGraphNode>(*newBlk));
            }
        }
    }
}

bool LoopInvariant::IsInSubloop(Loop *loop, CtrlFlowGraphNode *cfgNode) {
    for (auto &&sub_loop : loop->sub_loops) {
        auto &&blkInst = sub_loop->GetLoopBodyBlks();
        if (std::find(blkInst.begin(), blkInst.end(), cfgNode) != blkInst.end()) {
            return true;
        }
    }
    auto &&myBlkList = loop->GetEntireLoop();
    if (std::find(myBlkList.begin(), myBlkList.end(), cfgNode) == myBlkList.end()) {
        throw "Fail to find the block in this loop";
    }
    return false;
}

InvariantsInBlocks LoopInvariant::FindInvariant(Loop *loop) {
    LoopBlocks &&loop_blks = loop->GetEntireLoop();

    std::set<BaseValuePtr> defined_in_loop;
    std::set<InstPtr> instructions;
    std::unordered_map<BaseValuePtr, bool> isGeped;
    std::set<BaseValuePtr> gepedValue;
    InvariantsInBlocks invariants;

    for (auto &&blk : loop_blks) {
        for (auto &&inst : blk->GetInstList()) {
            if (inst->IsStoreInst()) {
                auto &&binary_inst = std::static_pointer_cast<BinaryInstruction>(inst);
                defined_in_loop.insert(binary_inst->GetLHS());
            } else if (inst->IsGepInst()) {
                isGeped[inst->GetResult()] = true;
                gepedValue.insert(inst->GetResult());
                defined_in_loop.insert(inst->GetResult());
            } else if (inst->GetResult()) {
                defined_in_loop.insert(inst->GetResult());
            }
        }
    }

    bool change = false;
    do {
        change = false;
        for (auto &&blk : loop_blks) {
            instructions.clear();
            if (IsInSubloop(loop, blk)) {
                continue;
            }
            for (auto &&inst : blk->GetInstList()) {
                bool invariant_check = true;
                if (inst->IsCallInst() || inst->IsJumpInst() || inst->IsReturnInst() || inst->IsBranchInst() ||
                    inst->IsPhiInst() || inst->IsGepInst()) {
                    continue;
                }
                if (inst->IsStoreInst()) {
                    auto &&binary_inst = std::static_pointer_cast<BinaryInstruction>(inst);

                    auto &&lhs = binary_inst->GetLHS();
                    auto &&rhs = binary_inst->GetRHS();

                    if (defined_in_loop.find(lhs) == defined_in_loop.end()) {
                        continue;
                    }
                    if (isGeped[lhs] == true && gepedValue.find(lhs) != gepedValue.end()) {
                        continue;
                    }

                    if (defined_in_loop.find(rhs) != defined_in_loop.end()) {
                        invariant_check = false;
                    }

                }

                else {
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
                        instructions.insert(inst);
                    } else {
                        defined_in_loop.erase(inst->GetResult());
                        instructions.insert(inst);
                    }
                    change = true;
                }
            }
            if (!instructions.empty()) {
                invariants.push_back({blk, instructions});
            }
        }
    } while (change);
    return invariants;
}