#pragma once

#include <list>
#include <memory>
#include <set>
#include <vector>
#include <unordered_map>

#include "3tle3wa/backend/Interface.hh"

class UopGeneral;
class VirtualRegister;
class CtrlFlowGraphNode;
class RLPlanner;
class AsmBasicBlock;

using CfgNodePtr = std::shared_ptr<CtrlFlowGraphNode>;

class RLBasicBlock : public Serializable {
    size_t lbidx_;
    size_t op_idx_;

    std::set<size_t> successors_;
    std::set<size_t> dominators_;
    std::set<size_t> predecessors_;

    std::set<size_t> live_out_;
    std::set<size_t> live_in_;
    std::set<size_t> live_use_;
    std::set<size_t> live_def_;

    std::list<std::unique_ptr<UopGeneral>> ops_;

    std::list<UopGeneral *> op_view_;

    RLPlanner *planner_;

    void formatString(FILE *fp) final;
    
   public:
    RLBasicBlock(CfgNodePtr &cfg, RLPlanner *planner);

    void Push(UopGeneral *uop);

    size_t GetLabelIdx() const;

    bool IsLiveOut(size_t vridx);

    void CalcuClear();
    
    void SetCalcuParam(std::vector<VirtualRegister *> &params);
    
    void CalcuInit();
    
    bool CalcuIO(std::unordered_map<size_t, RLBasicBlock *> &lbmap);

    void IndexAllUops();

    void CalcuInterval();

    void VirtualSchedule();

    void NoSchedule();

    void operator+=(RLBasicBlock &income);

    void ToAsm(AsmBasicBlock *abb, RLPlanner *plan);
};