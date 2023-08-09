#include "3tle3wa/backend/asm/AsmAbi.hh"
#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmGlobalValue.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"
#include "3tle3wa/backend/asm/AsmLocalConstant.hh"
#include "3tle3wa/backend/asm/AsmProgress.hh"
#include "3tle3wa/backend/rl/InternalTranslation.hh"
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

    do {
        std::unordered_map<size_t, UopCall *> call_map;
        for (auto &&rlbb : rlbbs_) {
            rlbb->GetCallMapInfo(call_map);
        }

        planner_->GenerateCallInfo(call_map);
    } while (0);

    apg->SetFirstBlk(rlbbs_.front()->GetLabelIdx());

    for (auto &&rlbb : rlbbs_) {
        auto abb = std::make_unique<AsmBasicBlock>(rlbb->GetLabelIdx(), apg);
        rlbb->ToAsm(abb.get(), planner_.get());
        apg->Push(std::move(abb), abb->IsRet());
    }

    apg->DoOptimization();
}

void RLBasicBlock::GetCallMapInfo(std::unordered_map<size_t, UopCall *> &call_map) {
    for (auto &&uop : uops_view_) {
        if (uop->IsCall()) {
            auto call = dynamic_cast<UopCall *>(uop);
            call_map.emplace(uop->GetUopIdx(), call);
        }
    }
}

void RLBasicBlock::ToAsm(AsmBasicBlock *abb, RLPlanner *plan) {
    auto end2 = --uops_view_.end();
    for (auto it = uops_view_.begin(); it != uops_view_.end(); ++it) {
        if (it == end2) {
            plan->RecoverCall(abb);
        }
        (*it)->ToAsm(abb, plan);
    }
}

void RLPlanner::GenerateCallInfo(const std::unordered_map<size_t, UopCall *> &call_map) {
    for (auto &&reg : vr_storage_) {
        if (not reg->IsOnStk() and (abi_reg_info.i.caller_save.count(reg->GetRealRegIdx()) or
                                    abi_reg_info.f.caller_save.count(reg->GetRealRegIdx()))) {
            for (auto &&[idx, call] : call_map) {
                if (reg->LiveAt(idx)) {
                    call->PushLiver(reg.get());
                }
            }
        }
    }
}
