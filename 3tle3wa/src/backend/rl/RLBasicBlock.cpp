#include "3tle3wa/backend/rl/RLBasicBlock.hh"

#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/ir/IR.hh"

RLBasicBlock::RLBasicBlock(CfgNodePtr &cfg, RLPlanner *planner) : lbidx_(cfg->GetBlockIdx()), op_idx_(0), planner_(planner) {
    auto &&succ_lst = cfg->GetSuccessors();
    for (auto &&succ : succ_lst) {
        successors_.insert(succ->GetBlockIdx());
    }

    auto &&domi_lst = cfg->GetDominatorSet();
    for (auto &&domi : domi_lst) {
        dominators_.insert(domi->GetBlockIdx());
    }

    auto &&pred_lst = cfg->GetPredcessors();
    for (auto &&pred : pred_lst) {
        predecessors_.insert(pred->GetBlockIdx());
    }
}

void RLBasicBlock::Push(UopGeneral *uop) { ops_.push_back(std::unique_ptr<UopGeneral>(uop)); }

size_t RLBasicBlock::GetLabelIdx() const { return lbidx_; }