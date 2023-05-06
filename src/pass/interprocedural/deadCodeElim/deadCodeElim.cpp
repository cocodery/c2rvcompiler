#include "deadCodeElim.hh"

void DeadCodeElimination::EliminateUselessCode(CfgNodeList allNodes) {
    std::map<InstPtr, bool> visitMap;
    std::queue<InstPtr> WorkList;

    auto Mark = [&visitMap, &allNodes, &WorkList]() {
        for (auto &&node : allNodes) {
            for (auto &&inst : node->GetInstList()) {
                if (inst->IsCriticalOperation()) {
                    visitMap[inst] = true;
                    WorkList.push(inst);
                }
            }
        }
        while (!WorkList.empty()) {
            auto &&front = WorkList.front();
            WorkList.pop();
            for (auto &&value : front->UsedValue()) {
                auto &&inst_def_value = value->GetParent();
                if (inst_def_value == nullptr) continue;
                if (visitMap[inst_def_value] == false) {
                    visitMap[inst_def_value] = true;
                    WorkList.push(inst_def_value);
                }
            }
        }
        assert(WorkList.empty());
    };

    auto Sweep = [&visitMap, &allNodes]() {
        for (auto &&node : allNodes) {
            auto &&inst_list = node->GetInstList();
            for (auto &&iter = inst_list.begin(); iter != inst_list.end();) {
                auto &&inst = (*iter);
                if (visitMap[inst] == false) {
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

void DeadCodeElimination::EliminateUselessControlFlow(NormalFuncPtr normal_func) {
    auto FoldRedundantBranch = [](CfgNodePtr i, CfgNodePtr j) {
        i->RemoveLastInst();
        i->InsertInstBack(JumpInst::CreatePtr(j, i));
    };

    auto RemoveEmptyBlock = [&normal_func](CfgNodePtr i, CfgNodePtr j) {
        if (i->FindBlkAttr(ENTRY)) {
            j->AppendBlkAttr(ENTRY);
            normal_func->SetEntryNode(j);
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

    auto CombineBlocks = [&normal_func](CfgNodePtr i, CfgNodePtr j) {
        i->RemoveLastInst();
        auto &&i_inst_list = i->GetInstList();
        auto &&j_inst_list = j->GetInstList();
        for_each(i_inst_list.begin(), i_inst_list.end(), [&j](const auto &inst) { inst->SetParent(j); });
        i_inst_list.insert(i_inst_list.end(), j_inst_list.begin(), j_inst_list.end());
        j_inst_list = std::move(i_inst_list);
        j->AppendBlkAttr(i->GetBlockAttr());
        // if (j->FindBlkAttr(GORETURN | EXIT)) j->ClearSpecAttr(GORETURN);
        if (j->FindBlkAttr(ENTRY)) normal_func->SetEntryNode(j);
        // ReplacePredSucc(i, j);
        for (auto &&pred : i->GetPredcessors()) {
            pred->AddSuccessor(j);
            j->AddPredcessor(pred);
            pred->GetLastInst()->ReplaceTarget(i, j);
        }
        RemoveNode(i);
    };

    auto HoistBranch = [](CfgNodePtr i, CfgNodePtr j) {
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

    auto Clean = [&OnePass, &normal_func]() {
        while (true) {
            auto &&post_order = normal_func->TopoSortFromExit();
            if (!OnePass(post_order)) break;
        }
    };

    Clean();
}

void DeadCodeElimination::EliminateUnreachableCode(CfgNodePtr exit, CfgNodeList allNodes) {
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