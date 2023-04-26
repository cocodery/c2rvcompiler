#include "deadCodeElim.hh"

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
    std::for_each(delNodeSet.begin(), delNodeSet.end(), RemoveSelf);
}