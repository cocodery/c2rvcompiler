#include "3tle3wa/backend/rl/RLProgress.hh"

#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/ir/IR.hh"

RLProgress::RLProgress(const std::string &name) : has_call_(false), has_lib_call_(false), has_call_other_(false) {
    FILE *fp = open_memstream(&label_, &label_len_);
    fprintf(fp, "%s", name.c_str());
    fflush(fp);
    fclose(fp);
}

void RLProgress::RegisterPlanner(std::unique_ptr<RLPlanner> &planner) {
    planner_ = std::move(planner);
    planner_->RegisterOwner(this);
}

void RLProgress::Push(std::unique_ptr<RLBasicBlock> &rlbb) { rlbbs_.push_back(std::move(rlbb)); }

void RLProgress::SetParam(Variable *var, VREG_TYPE type) {
    extern size_t abi_arg_reg;

    if (type == VREG_TYPE::FLT) {
        if (fps_ < abi_arg_reg) {
            auto param = planner_->AllocParam(type, var->GetVariableIdx(), 8, false, fps_);
            params_.push_back(param);
            fps_ += 1;
            return;
        }
    } else {
        if (ips_ < abi_arg_reg) {
            auto param = planner_->AllocParam(type, var->GetVariableIdx(), 8, false, ips_);
            params_.push_back(param);
            ips_ += 1;
            return;
        }
    }
    auto param = planner_->AllocParam(type, var->GetVariableIdx(), 8, true, sps_);
    params_.push_back(param);
    sps_ += 1;
}

void RLProgress::MeetCall() { has_call_ = true; }

void RLProgress::MeetLibCall() { has_lib_call_ = true; }

void RLProgress::MeetCallOther() { has_call_other_ = true; }

bool RLProgress::HasCallFunc() { return has_call_; }

RLBasicBlock *RLProgress::FindBlkById(size_t lbidx) { return lbmap_.at(lbidx); }