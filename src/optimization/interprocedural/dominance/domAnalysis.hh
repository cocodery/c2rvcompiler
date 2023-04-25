#pragma once

#include <list>
#include <queue>

#include "IR.hh"

class DomAnalysis {
   private:
    CfgNodePtr entry;
    CfgNodeList allNodes;

   public:
    DomAnalysis(CfgNodePtr, CfgNodeList);
    ~DomAnalysis() = default;

    void DominanceAnalysis();
    void DominanceFrontier();

    void DoDomAnalysis();
};

void DoDomAnalysis(CfgNodePtr, CfgNodeList);