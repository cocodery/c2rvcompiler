#pragma once

#include <list>
#include <memory>
#include <set>
#include <vector>

class UopGeneral;
class CfgNodePtr;

class RLBasicBlock {
    size_t lbidx_;

    std::set<size_t> successors_;
    std::set<size_t> dominators_;
    std::set<size_t> immdiate_dominators_;
    std::set<size_t> predecessors_;

    std::set<size_t> live_out_;
    std::set<size_t> live_in_;
    std::set<size_t> ue_var_;
    std::set<size_t> var_kill_;

    std::list<std::unique_ptr<UopGeneral>> ops_;

   public:
    RLBasicBlock(CfgNodePtr &cfg);

    void Push(UopGeneral *);
};