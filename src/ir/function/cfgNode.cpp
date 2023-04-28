#include "cfgNode.hh"

CtrlFlowGraphNode::CtrlFlowGraphNode() : BasicBlock() {}

bool CtrlFlowGraphNode::GetDirty() { return dirty; }
void CtrlFlowGraphNode::SetDirty(bool _dirty) { dirty = _dirty; }

CfgNodePtr CtrlFlowGraphNode::CreatePtr() { return std::make_shared<CtrlFlowGraphNode>(); }

void CtrlFlowGraphNode::AddPredcessor(CfgNodePtr predecessor) { predecessors.push_back(predecessor); }
void CtrlFlowGraphNode::AddSuccessor(CfgNodePtr successor) { successors.push_back(successor); }
CfgNodeList &CtrlFlowGraphNode::GetPredcessors() { return predecessors; }
CfgNodeList &CtrlFlowGraphNode::GetSuccessors() { return successors; }

void CtrlFlowGraphNode::InsertDominator(CfgNodePtr dominator) { dominator_set.insert(dominator); }
DominatorSet &CtrlFlowGraphNode::GetDominatorSet() { return dominator_set; }
void CtrlFlowGraphNode::SetDominatorSet(DominatorSet &_dom_set) {
    dominator_set.clear();
    dominator_set = _dom_set;
}

CfgNodePtr CtrlFlowGraphNode::GetImmediateDominator() { return immediate_dominator; }
void CtrlFlowGraphNode::SetImmediateDominator(CfgNodePtr _idom) { immediate_dominator = _idom; }

void CtrlFlowGraphNode::InsertDomFrontier(CfgNodePtr frontier) { dominance_frontier.insert(frontier); }
DominatorSet &CtrlFlowGraphNode::GetDomFrontier() { return dominance_frontier; }

std::string CtrlFlowGraphNode::tollvmIR() {
    std::stringstream ss;

    ss << "Block_" << idx << ":" << endl;
    ss << "\t; Predecessors: ";
    for (auto &&pred : predecessors) {
        ss << pred->idx << ' ';
    }
    ss << '\n';

    ss << "\t; Successors  : ";
    for (auto &&succ : successors) {
        ss << succ->idx << ' ';
    }
    ss << '\n';

    ss << "\t; DominatorSet  : ";
    for (auto &&node : dominator_set) {
        ss << node->idx << ' ';
    }
    ss << '\n';

    ss << "\t; ImmediateDominator  : ";
    if (immediate_dominator != nullptr) {
        ss << immediate_dominator->idx;
    }
    ss << '\n';

    ss << "\t; DominanceFrontier  : ";
    for (auto &&node : dominance_frontier) {
        ss << node->idx << ' ';
    }
    ss << '\n';

    for (auto &&inst : inst_list) {
        ss << '\t' << inst->tollvmIR() << endl;
    }

    return ss.str();
}

// clear instructions of del-node
// remove del-node from its predcessors and successors
// clear predcessors and successors of del-node
void RemoveNode(CfgNodePtr node) {
    auto &&inst_list = node->GetInstList();
    std::for_each(inst_list.begin(), inst_list.end(), RemoveInst);
    inst_list.clear();
    auto &&predcessor = node->GetPredcessors();
    auto &&successor = node->GetSuccessors();
    std::for_each(predcessor.begin(), predcessor.end(),
                  [&node](const auto &pred) { pred->GetSuccessors().remove(node); });
    std::for_each(successor.begin(), successor.end(),
                  [&node](const auto &succ) { succ->GetPredcessors().remove(node); });
    predcessor.clear();
    successor.clear();
}
