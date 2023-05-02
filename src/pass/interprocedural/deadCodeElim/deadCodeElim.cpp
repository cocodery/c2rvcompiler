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