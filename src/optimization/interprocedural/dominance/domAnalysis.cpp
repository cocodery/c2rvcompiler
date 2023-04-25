#include "domAnalysis.hh"

DomAnalysis::DomAnalysis(CfgNodePtr _entry, CfgNodeList(_allNodes)) : entry(_entry), allNodes(_allNodes) {}

void DomAnalysis::DominanceAnalysis() {
    entry->InsertDominator(entry);
    DominatorSet allNodeSet = DominatorSet(allNodes.begin(), allNodes.end());

    for (auto &&node : allNodes) {
        if (node == entry) continue;
        node->SetDominatorSet(allNodeSet);
    }

    auto &&IntersectPredecessorsDom = [&](CfgNodePtr node) {
        DominatorSet dom_set = allNodeSet;

        for (auto &&pred : node->GetPredcessors()) {
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

    auto &&ComputeImmediateDominator = [&](CfgNodePtr node) {
        DominatorSet dominator_set = node->GetDominatorSet();
        for (auto &&dominator : dominator_set) {
            if (dominator == node) continue;

            DominatorSet dom_set_of_dom = dominator->GetDominatorSet();
            dom_set_of_dom.insert(node);
            if (dom_set_of_dom == dominator_set) {
                return dominator;
            }
        }
        // unreachable!
        assert(false);
    };

    for (auto &&node : allNodes) {
        if (node == entry) continue;
        node->SetImmediateDominator(ComputeImmediateDominator(node));
    }
}

void DomAnalysis::DominanceFrontier() {
    for (auto &&node : allNodes) {
        node->GetDomFrontier().clear();
    }

    for (auto &&node : allNodes) {
        if (node->GetPredcessors().size() > 1) {
            for (auto &&pred : node->GetPredcessors()) {
                CfgNodePtr runner = pred;
                while (runner != node->GetImmediateDominator()) {
                    runner->InsertDomFrontier(node);
                    runner = runner->GetImmediateDominator();
                }
            }
        }
    }
}

void DomAnalysis::DoDomAnalysis() {
    DominanceAnalysis();
    DominanceFrontier();
}

void DoDomAnalysis(CfgNodePtr _entry, CfgNodeList _allNodes) {
    DomAnalysis dominance = DomAnalysis(_entry, _allNodes);
    dominance.DoDomAnalysis();
}