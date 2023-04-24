#pragma once

#include <list>
#include <map>
#include <queue>

#include "../../../include/IR.hh"

class DomAnalysis {
   private:
    CfgNodePtr entry;
    CfgNodeList allNodes;

   public:
    DomAnalysis(CfgNodePtr);
    ~DomAnalysis() = default;

    void DoDomAnalysis();
};

void DoDomAnalysis(NormalFuncPtr);