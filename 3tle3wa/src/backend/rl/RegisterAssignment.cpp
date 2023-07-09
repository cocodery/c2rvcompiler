#include <unordered_map>
#include <unordered_set>

#include "3tle3wa/backend/InternalTranslation.hh"
#include "3tle3wa/backend/IntervalTree.hh"
#include "3tle3wa/backend/asm/AsmGlobalValue.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLGen.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/backend/utils.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/instruction/opCode.hh"

void InternalTranslation::DoAssignment() { rlps_->DoAssignment(); }

void RLProgress::DoAssignment() {
    computeLivenessInfomation();

    computeLiveInterval();

    rvAssigner();
}

void RLProgress::computeLivenessInfomation() {
    size_t pridx = 0;
    for (auto &&bb : rlbbs_) {
        CRVC_UNUSE auto result = lbmap_.emplace(bb->GetLabelIdx(), bb.get());
        Assert(result.second, "fail");

        bb->CalcuClear();

        if (pridx == 0) {
            bb->SetCalcuParam(params_);
        }

        bb->CalcuInit();

        pridx += 1;
    }

    bool changed = false;
    do {
        changed = false;
        for (auto &&rit = rlbbs_.rbegin(); rit != rlbbs_.rend(); ++rit) {
            changed = (*rit)->CalcuIO(lbmap_) || changed;
        }
    } while (changed);

    (*rlbbs_.begin())->SetLiveinParam(params_);
}

void RLBasicBlock::CalcuClear() {
    live_def_.clear();
    live_use_.clear();
    live_out_.clear();
    live_in_.clear();
}

void RLBasicBlock::SetCalcuParam(std::vector<VirtualRegister *> &params) {
    for (auto &&param : params) {
        live_def_.insert(param->GetVRIdx());
    }
}

void RLBasicBlock::SetLiveinParam(std::vector<VirtualRegister *> &params) {
    for (auto &&param : params) {
        live_in_.insert(param->GetVRIdx());
    }
}

void RLBasicBlock::CalcuInit() {
    for (auto &&uop : ops_) {
        auto operands = uop->GetOperands();
        if (not operands.empty()) {
            for (auto &&operand : operands) {
                auto vridx = operand->GetVRIdx();
                if (live_def_.find(vridx) == live_def_.end()) {
                    live_use_.insert(vridx);
                }
            }
        }

        auto result = uop->GetResult();
        if (result != nullptr) {
            live_def_.insert(result->GetVRIdx());
        }
    }
}

void RLBasicBlock::operator+=(RLBasicBlock &income) {
    live_out_.insert(income.live_in_.begin(), income.live_in_.end());
}

bool RLBasicBlock::CalcuIO(std::unordered_map<size_t, RLBasicBlock *> &lbmap) {
    size_t prev_size = live_out_.size();

    for (auto &&succ : successors_) {
        auto succbb = lbmap.at(succ);
        *this += *succbb;
    }
    live_in_ = live_use_;
    std::set_difference(live_out_.begin(), live_out_.end(), live_def_.begin(), live_def_.end(),
                        std::inserter(live_in_, live_in_.end()));

    return prev_size != live_out_.size();
}

void RLProgress::computeLiveInterval() {
    for (auto &&rit = rlbbs_.rbegin(); rit != rlbbs_.rend(); ++rit) {
        auto &&bb = (*rit);
        bb->IndexAllUops();
        bb->CalcuInterval();
    }
}

void RLBasicBlock::IndexAllUops() {
    for (auto &&uop : ops_) {
        op_idx_ += 1;
        uop->SetUopIdx(op_idx_);
    }
    for (auto &&regid : live_out_) {
        auto vr = planner_->GetVReg(regid);

        vr->NewInterval(lbidx_, 0, op_idx_ + 1, op_idx_ + 1);
    }
}

void RLBasicBlock::CalcuInterval() {
    std::vector<UopGeneral *> callidx;
    std::vector<VirtualRegister *> scope_vr;

    for (auto &&in : live_in_) {
        scope_vr.push_back(planner_->GetVReg(in));
    }

    for (auto &&rit = ops_.rbegin(); rit != ops_.rend(); rit++) {
        auto &&uop = *rit;

        if (uop->GetOpKind() == OPERATION_KIND::CALLOP) {
            callidx.push_back(rit->get());
        }

        auto result = uop->GetResult();
        if (result != nullptr) {
            result->UpdateIntervalBegin(lbidx_, uop->GetUopIdx(), op_idx_ + 1);
            scope_vr.push_back(result);
        }

        auto &&operands = uop->GetOperands();
        if (not operands.empty()) {
            for (auto &&operand : operands) {
                operand->UpdateIntervalEnd(lbidx_, 0, uop->GetUopIdx(), op_idx_ + 1);
                operand->UseAt(lbidx_);
                scope_vr.push_back(operand);
            }
        }
    }

    // for param save
    for (auto &&uop : callidx) {
        auto calli = dynamic_cast<UopCall *>(uop);
        Assert(calli, "bad dynamic cast");
        for (auto &&vr : scope_vr) {
            auto intval = vr->Imgr().AskInterval(lbidx_);
            if (intval != nullptr && (*intval)[uop->GetUopIdx()]) {
                calli->PushLiver(vr);
            }
        }
        calli->BroadCastCall(lbidx_);
    }
}
