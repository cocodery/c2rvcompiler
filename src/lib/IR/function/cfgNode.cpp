#include "cfgNode.hh"

CtrlFlowGraphNode::CtrlFlowGraphNode() : BasicBlock() {}

CfgNodePtr CtrlFlowGraphNode::CreatePtr() { return std::make_shared<CtrlFlowGraphNode>(); }

void CtrlFlowGraphNode::AddPredcessor(CfgNodePtr predecessor) { predecessors.push_back(predecessor); }
void CtrlFlowGraphNode::AddSuccessor(CfgNodePtr successor) { successors.push_back(successor); }

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

    for (auto &&inst : inst_list) {
        ss << '\t' << inst->tollvmIR() << endl;
    }

    return ss.str();
}
