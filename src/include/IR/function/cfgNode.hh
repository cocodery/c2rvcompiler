#pragma once

#include "basicblock.hh"

class CtrlFlowGraphNode;
using CfgNodePtr = std::shared_ptr<CtrlFlowGraphNode>;

class CtrlFlowGraphNode : public BasicBlock {
   private:
    std::list<CfgNodePtr> predecessors;
    std::list<CfgNodePtr> successors;

   public:
    CtrlFlowGraphNode();
    ~CtrlFlowGraphNode() = default;

    void AddPredcessor(CfgNodePtr);
    void AddSuccessor(CfgNodePtr);

    static CfgNodePtr CreatePtr();

    std::string tollvmIR();
};
