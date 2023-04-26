#include "getAllNodes.hh"

CfgNodeList GetAllNodes::GetAllNodes(CfgNodePtr entry) {
    CfgNodeList allNodes;

    // Solve the control flow graph from entry
    // Obtain all the nodes that the function reaches
    std::queue<CfgNodePtr> nodeQueue;
    nodeQueue.push(entry);
    while (!nodeQueue.empty()) {
        auto &&front = nodeQueue.front();
        nodeQueue.pop();
        // if queue-front not in visitMap
        if (std::find(allNodes.begin(), allNodes.end(), front) == allNodes.end()) {
            allNodes.push_back(front);
            for (auto &&node : front->GetSuccessors()) {
                nodeQueue.push(node);
            }
        }
    }
    assert(nodeQueue.empty());

    return allNodes;
}
