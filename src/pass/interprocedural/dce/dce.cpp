#include "dce.hh"

void DCE::EliminateUselessCode(NormalFuncPtr func) {
    auto allNodes = func->TopoSortFromEntry();

    std::set<InstPtr> visitSet;
    std::queue<InstPtr> WorkList;

    auto Mark = [&visitSet, &allNodes, &WorkList]() {
        for (auto &&node : allNodes) {
            for (auto &&inst : node->GetInstList()) {
                if (inst->IsCriticalOperation()) {
                    visitSet.insert(inst);
                    WorkList.push(inst);
                }
            }
        }
        while (!WorkList.empty()) {
            auto &&front = WorkList.front();
            WorkList.pop();
            for (auto &&value : front->GetOprands()) {
                auto &&inst_def_value = value->GetParent();
                if (inst_def_value == nullptr) continue;
                if (visitSet.find(inst_def_value) == visitSet.end()) {
                    visitSet.insert(inst_def_value);
                    WorkList.push(inst_def_value);
                }
            }
        }
        assert(WorkList.empty());
    };

    auto Sweep = [&visitSet, &allNodes]() {
        for (auto &&node : allNodes) {
            auto &&inst_list = node->GetInstList();
            for (auto &&iter = inst_list.begin(); iter != inst_list.end();) {
                auto &&inst = (*iter);
                if (visitSet.find(inst) == visitSet.end()) {
                    if (inst->IsJumpInst() == false) {
                        RemoveInst(inst);
                        iter = inst_list.erase(iter);
                        continue;
                    }
                }
                ++iter;
            }
        }
    };

    Mark();
    Sweep();
}

void DCE::EliminateUselessControlFlow(NormalFuncPtr func) {
    auto FoldRedundantBranch = [](CfgNodePtr i, CfgNodePtr j) {
        i->RemoveLastInst();
        i->InsertInstBack(JumpInst::CreatePtr(j, i));
    };

    auto RemoveEmptyBlock = [&func](CfgNodePtr i, CfgNodePtr j) {
        if (i->FindBlkAttr(ENTRY)) {
            j->AppendBlkAttr(ENTRY);
            func->SetEntryNode(j);
            RemoveNode(i);
            return true;
        } else {
            // for (auto &&pred : i->GetPredcessors()) {
            //     pred->AddSuccessor(j);
            //     j->AddPredcessor(pred);
            //     pred->GetLastInst()->ReplaceTarget(i, j);
            // }
            // RemoveNode(i);
            // return true;
        }
        return false;
    };

    auto CombineBlocks = [&func](CfgNodePtr i, CfgNodePtr j) {
        i->RemoveLastInst();
        auto &&i_inst_list = i->GetInstList();
        auto &&j_inst_list = j->GetInstList();
        for_each(i_inst_list.begin(), i_inst_list.end(), [&j](const auto &inst) { inst->SetParent(j); });
        i_inst_list.insert(i_inst_list.end(), j_inst_list.begin(), j_inst_list.end());
        j_inst_list = std::move(i_inst_list);
        j->AppendBlkAttr(i->GetBlockAttr());
        // if (j->FindBlkAttr(GORETURN | EXIT)) j->ClearSpecAttr(GORETURN);
        if (j->FindBlkAttr(ENTRY)) func->SetEntryNode(j);
        // ReplacePredSucc(i, j);
        for (auto &&pred : i->GetPredcessors()) {
            pred->AddSuccessor(j);
            j->AddPredcessor(pred);
            pred->GetLastInst()->ReplaceTarget(i, j);
        }
        RemoveNode(i);
    };

    [[maybe_unused]] auto HoistBranch = [](CfgNodePtr i, CfgNodePtr j) {
        auto &&j_branch_inst = std::static_pointer_cast<BranchInst>(j->GetLastInst());
        auto &&i_branch_inst = BranchInst::CreatePtr(j_branch_inst->GetCondition(), j_branch_inst->GetTrueTarget(),
                                                     j_branch_inst->GetFalseTarget(), i);
        i->RemoveLastInst();
        i->InsertInstBack(i_branch_inst);
        i->GetSuccessors().remove(j);
        j->GetPredcessors().remove(i);
    };

    auto OnePass = [&](CfgNodeList post_order) {
        bool changed = false;
        for (auto &&iter = post_order.begin(); iter != post_order.end();) {
            auto &&i = (*iter);
            auto &&last_inst = i->GetLastInst();
            if (last_inst->IsBranchInst()) {
                // case 1, fold redundant branch
                auto &&branch_inst = std::static_pointer_cast<BranchInst>(last_inst);
                auto &&iftrue = branch_inst->GetTrueTarget();
                auto &&iffalse = branch_inst->GetFalseTarget();
                if (iftrue == iffalse) {
                    FoldRedundantBranch(i, iftrue);
                }
            } else if (last_inst->IsJumpInst()) {
                auto &&jump_inst = std::static_pointer_cast<JumpInst>(last_inst);
                auto &&j = jump_inst->GetTarget();
                // case 2, remove empty block
                if (i->GetInstCnt() == 1 && !j->FindBlkAttr(LOOPBEGIN)) {
                    if (RemoveEmptyBlock(i, j)) {
                        iter = post_order.erase(iter);
                        continue;
                    }
                }
                // case 3, combine i and j
                if (j->GetPredcessors().size() == 1) {
                    CombineBlocks(i, j);
                    iter = post_order.erase(iter);
                    continue;
                }
                // case 4, hoist a branch
                if (j->GetInstCnt() == 1 && j->GetLastInst()->IsBranchInst()) {
                    // HoistBranch(i, j);
                }
            }
            ++iter;
        }
        return changed;
    };

    auto Clean = [&OnePass, &func]() {
        while (true) {
            auto &&post_order = func->TopoSortFromExit();
            if (!OnePass(post_order)) break;
        }
    };

    Clean();
}

void DCE::EliminateUnreachableCode(NormalFuncPtr func) {
    auto exit = func->GetExitNode();
    auto allNodes = func->TopoSortFromEntry();
    // Solve the control flow graph from exit
    // Identify the unreachable nodes in the control flow
    std::queue<CfgNodePtr> nodeQueue;
    std::map<CfgNodePtr, bool> visitMap;
    std::set<CfgNodePtr> delNodeSet;
    nodeQueue.push(exit);
    while (!nodeQueue.empty()) {
        auto &&front = nodeQueue.front();
        nodeQueue.pop();
        if (visitMap[front] == false) {
            visitMap[front] = true;
            for (auto &&node : front->GetPredcessors()) {
                nodeQueue.push(node);
                if (std::find(allNodes.begin(), allNodes.end(), node) == allNodes.end()) {
                    delNodeSet.insert(node);
                }
            }
        }
    }
    assert(nodeQueue.empty());

    // remove dead-node
    std::for_each(delNodeSet.begin(), delNodeSet.end(), RemoveNode);
}