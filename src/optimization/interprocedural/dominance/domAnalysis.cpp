#include "domAnalysis.hh"

DomAnalysis::DomAnalysis(CfgNodePtr _entry) : entry(_entry) {
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
}

void DomAnalysis::DoDomAnalysis() {
    entry->InsertDominator(entry);
    DominatorSet allNodeSet = DominatorSet(allNodes.begin(), allNodes.end());

    for (auto &&node : allNodes) {
        if (node == entry) continue;
        node->SetDominatorSet(allNodeSet);
    }

    std::function<DominatorSet(CfgNodePtr)> IntersectPredecessorsDom = [&](CfgNodePtr node) {
        DominatorSet dom_set = allNodeSet;

        for (auto &&pred : node->GetPredcessors()) {
            // Not In AllNodes, Dead Code, Ingore It !
            if (std::find(allNodes.begin(), allNodes.end(), pred) == allNodes.end()) continue;

            DominatorSet temp = DominatorSet();  // To Collect Intersection
            DominatorSet dom_set_pred = pred->GetDominatorSet();
            std::set_intersection(dom_set.begin(), dom_set.end(), dom_set_pred.begin(), dom_set_pred.end(),
                                  std::inserter(temp, temp.begin()));
            dom_set = temp;
        }
        dom_set.insert(node);
        return dom_set;
    };

    bool changed = true;
    while (changed) {
        changed = false;

        for (auto &&node : allNodes) {
            if (node == entry) continue;

            DominatorSet temp = IntersectPredecessorsDom(node);
            if (temp != node->GetDominatorSet()) {
                node->SetDominatorSet(temp);
                changed = true;
            }
        }
    }
}

void DoDomAnalysis(NormalFuncPtr normal_func) {
    DomAnalysis dominance = DomAnalysis(normal_func->GetEntryNode());
    dominance.DoDomAnalysis();
}