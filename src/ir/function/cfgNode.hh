#pragma once

#include <set>

#include "basicblock.hh"
#include "controlFlowInst.hh"

class CtrlFlowGraphNode;
using CfgNodePtr = std::shared_ptr<CtrlFlowGraphNode>;
using CfgNodeList = std::list<CfgNodePtr>;
using DominatorSet = std::set<CfgNodePtr>;

class CtrlFlowGraphNode : public BasicBlock {
   private:
    bool dirty;

    CfgNodeList predecessors;
    CfgNodeList successors;

    DominatorSet dominator_set;

    CfgNodePtr immediate_dominator;
    DominatorSet dominate_children;

    DominatorSet dominance_frontier;

   public:
    CtrlFlowGraphNode(BlockAttr);
    ~CtrlFlowGraphNode() = default;

    bool GetDirty();
    void SetDirty(bool);

    void AddPredcessor(CfgNodePtr);
    void AddSuccessor(CfgNodePtr);
    CfgNodeList &GetPredcessors();
    CfgNodeList &GetSuccessors();

    void InsertDominator(CfgNodePtr);
    DominatorSet &GetDominatorSet();
    void SetDominatorSet(DominatorSet &);

    CfgNodePtr GetImmediateDominator();
    void SetImmediateDominator(CfgNodePtr);

    DominatorSet &GetDominateChildren();
    void InsertDominateChild(CfgNodePtr);

    void InsertDomFrontier(CfgNodePtr);
    DominatorSet &GetDomFrontier();

    static CfgNodePtr CreatePtr(BlockAttr);

    std::string tollvmIR();
};

void RemoveNode(CfgNodePtr node);
