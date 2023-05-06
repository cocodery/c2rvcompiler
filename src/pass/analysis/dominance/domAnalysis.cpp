#include "domAnalysis.hh"

void DominanceAnalysis::ComputeDominanceInfo(CfgNodePtr entry, CfgNodeList allNodes) {
    // dominator-set
    DominatorSet allNodeSet = DominatorSet(allNodes.begin(), allNodes.end());

    auto InitDominatorSet = [&entry, &allNodes, &allNodeSet]() {
        entry->InsertDominator(entry);

        for (auto &&node : allNodes) {
            if (node == entry) continue;
            node->SetDominatorSet(allNodeSet);
        }
    };

    auto ComputeDominatorSet = [&entry, &allNodes, &allNodeSet] {
        auto &&IntersectPredecessorsDom = [&](CfgNodePtr node) {
            DominatorSet dom_set = allNodeSet;
            auto &&predcessors = node->GetPredcessors();
            std::for_each(predcessors.begin(), predcessors.end(), [&dom_set](const auto &pred) {
                DominatorSet temp = DominatorSet();  // To Collect Intersection
                DominatorSet dom_set_pred = pred->GetDominatorSet();
                std::set_intersection(dom_set.begin(), dom_set.end(), dom_set_pred.begin(), dom_set_pred.end(),
                                      std::inserter(temp, temp.begin()));
                dom_set = temp;
            });
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
    };

    InitDominatorSet();
    ComputeDominatorSet();

    auto ComputeImmediateDominator = [&entry](const auto &node) {
        DominatorSet dominator_set = node->GetDominatorSet();
        for (auto &&dominator : dominator_set) {
            if (dominator == node) continue;
            DominatorSet dom_set_of_dom = dominator->GetDominatorSet();
            dom_set_of_dom.insert(node);
            if (dom_set_of_dom == dominator_set) return dominator;
        }
        assert(false);
    };

    for (auto &&node : allNodes) {
        if (node == entry) continue;
        node->SetImmediateDominator(ComputeImmediateDominator(node));
    }
}

void DominanceAnalysis::ComputeDominanceFrontier(CfgNodeList allNodes) {
    std::for_each(allNodes.begin(), allNodes.end(), [](const auto &node) { node->GetDomFrontier().clear(); });
    std::for_each(allNodes.begin(), allNodes.end(), [](const auto &node) {
        if (node->GetPredcessors().size() > 1) {
            for (auto &&pred : node->GetPredcessors()) {
                CfgNodePtr runner = pred;
                while (runner != node->GetImmediateDominator()) {
                    runner->InsertDomFrontier(node);
                    runner = runner->GetImmediateDominator();
                }
            }
        }
    });
}
