#include "preprocess.hh"

CfgNodeList DoPreProcess(CfgNodePtr entry, CfgNodePtr exit) {
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

    // Solve the control flow graph from exit
    // Identify the unreachable nodes in the control flow
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

    // clear instructions of del-node
    // remove del-node from its predcessors and successors
    // clear predcessors and successors of del-node
    for (auto &&node : delNodeSet) {
        node->getInstList().clear();
        for (auto &&pred : node->GetPredcessors()) {
            pred->GetSuccessors().remove(node);
        }
        for (auto &&succ : node->GetSuccessors()) {
            succ->GetPredcessors().remove(node);
        }
        node->GetPredcessors().clear();
        node->GetSuccessors().clear();
    }
    delNodeSet.clear();

    return allNodes;
}
