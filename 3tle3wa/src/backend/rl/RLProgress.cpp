#include "3tle3wa/backend/rl/RLProgress.hh"

#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/ir/IR.hh"

RLProgress::RLProgress(const std::string &name) {
    FILE *fp = open_memstream(&label_, &label_len_);
    fprintf(fp, "%s", name.c_str());
    fflush(fp);
    fclose(fp);
}

void RLProgress::RegisterPlanner(std::unique_ptr<RLPlanner> &planner) { planner_ = std::move(planner); }

void RLProgress::Push(std::unique_ptr<RLBasicBlock> &rlbb) { rlbbs_.push_back(std::move(rlbb)); }

void RLProgress::SetParam(Variable *var, VREG_TYPE type) {
    auto param = planner_->AllocVReg(type, var->GetVariableIdx());
    params_.push_back(param);
}