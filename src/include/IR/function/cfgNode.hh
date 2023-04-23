#pragma once

#include <set>

#include "basicblock.hh"

class CtrlFlowGraphNode;
using CfgNodePtr = std::shared_ptr<CtrlFlowGraphNode>;
using CfgNodeList = std::list<CfgNodePtr>;

class CtrlFlowGraphNode : public BasicBlock {
   private:
    CfgNodeList predecessors;
    CfgNodeList successors;

    std::set<CfgNodePtr> dom;

   public:
    CtrlFlowGraphNode();
    ~CtrlFlowGraphNode() = default;

    void AddPredcessor(CfgNodePtr);
    void AddSuccessor(CfgNodePtr);

    CfgNodeList &GetPredcessors();
    CfgNodeList &GetSuccessors();

    static CfgNodePtr CreatePtr();

    std::string tollvmIR();
};
