#pragma once

#include <set>

#include "basicblock.hh"

class CtrlFlowGraphNode;
using CfgNodePtr = std::shared_ptr<CtrlFlowGraphNode>;
using CfgNodeList = std::list<CfgNodePtr>;
using DominatorSet = std::set<CfgNodePtr>;

class CtrlFlowGraphNode : public BasicBlock {
   private:
    CfgNodeList predecessors;
    CfgNodeList successors;

    DominatorSet dominator_set;

   public:
    CtrlFlowGraphNode();
    ~CtrlFlowGraphNode() = default;

    void AddPredcessor(CfgNodePtr);
    void AddSuccessor(CfgNodePtr);

    CfgNodeList &GetPredcessors();
    CfgNodeList &GetSuccessors();

    void InsertDominator(CfgNodePtr);
    DominatorSet &GetDominatorSet();
    void SetDominatorSet(DominatorSet &);

    static CfgNodePtr CreatePtr();

    std::string tollvmIR();
};
