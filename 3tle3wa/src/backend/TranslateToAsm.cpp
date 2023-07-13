#include "3tle3wa/backend/InternalTranslation.hh"
#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmGlobalValue.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"
#include "3tle3wa/backend/asm/AsmLocalConstant.hh"
#include "3tle3wa/backend/asm/AsmProgress.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/utils/logs.hh"

void InternalTranslation::DoTranslateToAsm() { rlps_->DoToAsm(apg_.get()); }

void RLProgress::DoToAsm(AsmProgress *apg) {
    auto init = apg->CreateEntryBlock();
    planner_->Init(init);

    apg->SetFirstBlk(rlbbs_.front()->GetLabelIdx());

    for (auto &&rlbb : rlbbs_) {
        auto abb = std::make_unique<AsmBasicBlock>(rlbb->GetLabelIdx(), apg);
        rlbb->ToAsm(abb.get(), planner_.get());
        apg->Push(abb, abb->IsRet());
    }

    apg->DoOptimization();
}

void RLBasicBlock::ToAsm(AsmBasicBlock *abb, RLPlanner *plan) {
    for (auto &&uop : ops_) {
        if (uop->GetUopIdx() == op_idx_) {
            plan->RecoverCall(abb, uop->GetOperands());
        }
        uop->ToAsm(abb, plan);
    }
}