#pragma once

#include <list>
#include <memory>
#include <set>
#include <vector>

#include "3tle3wa/backend/Interface.hh"

class UopGeneral;
class CtrlFlowGraphNode;

using CfgNodePtr = std::shared_ptr<CtrlFlowGraphNode>;

class RLBasicBlock : public Serializable {
    size_t lbidx_;

    std::set<size_t> successors_;
    std::set<size_t> dominators_;
    std::set<size_t> predecessors_;

    std::set<size_t> live_out_;
    std::set<size_t> live_in_;
    std::set<size_t> ue_var_;
    std::set<size_t> var_kill_;

    std::list<std::unique_ptr<UopGeneral>> ops_;

    std::list<UopGeneral *> op_view_;

    void formatString(FILE *fp) final;
    
   public:
    RLBasicBlock(CfgNodePtr &cfg);

    void Push(UopGeneral *uop);

    void VirtualSchedule();

    void RealSchedule();

    void NoSchedule();
};