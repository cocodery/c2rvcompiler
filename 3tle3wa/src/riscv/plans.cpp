#include <queue>
#include <unordered_set>

#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/riscv/spec.hh"
#include "3tle3wa/backend/utils.hh"

void RLPlanner::PlanRegisters(size_t igpr[], size_t igprlen, size_t fgpr[], size_t fgprlen) {
    std::priority_queue<VirtualRegister *> vrpq;
    std::priority_queue<VirtualRegister *> spill;

    for (auto &&vr : vr_storage_) {
        if (vr->IsAssigned()) {
            if (vr->IsParam() and not vr->OnStk()) {
                if (vr->FGPR()) {
                    tryUse(vr.get(), riscv::fa0 + vr->GetPPos());
                } else {
                    tryUse(vr.get(), riscv::a0 + vr->GetPPos());
                }
            }
            continue;
        }
        vr->CalcuWeight();
        vrpq.push(vr.get());
    }

    while (not vrpq.empty()) {
        size_t *agpr;
        size_t alen;

        auto top = vrpq.top();
        vrpq.pop();

        if (top->FGPR()) {
            agpr = fgpr;
            alen = fgprlen;
        } else {
            agpr = igpr;
            alen = igprlen;
        }

        bool success = false;
        for (size_t i = 0; i < alen; ++i) {
            if (success = tryUse(top, agpr[i]); success) {
                break;
            }
        }

        if (not success) {
            spill.push(top);
        }
    }

    //
    // for now, no split
    //

    std::priority_queue<VirtualRegister *> spill2;

    while (not spill.empty()) {
        auto top = spill.top();
        spillOn(top);
        spill.pop();
    }
}

bool RLPlanner::tryUse(VirtualRegister *vr, size_t rridx) {
    IntervalManager *imgr = nullptr;
    if (auto fnd = real_reg_inval_.find(rridx); fnd == real_reg_inval_.end()) {
        auto mgr = std::make_unique<IntervalManager>();
        imgr = mgr.get();
        CRVC_UNUSE auto result = real_reg_inval_.emplace(rridx, std::move(mgr));
        Assert(result.second, "emplace failed");
    } else {
        imgr = fnd->second.get();
    }

    if (vr->Imgr() && *imgr) {
        return false;
    }

    *imgr |= vr->Imgr();
    vr->SetRRidx(rridx);

    return true;
}

void RLPlanner::spillOn(VirtualRegister *vr) {
    bool allocated = false;

    for (size_t i = 0; i < real_stk_inval_.size(); ++i) {
        auto &&imgr = real_stk_inval_[i];
        if (vr->Imgr() && *imgr) {
            continue;
        }

        auto sinfo = real_stkinfo_[i];

        vr->SetOnStack(true);
        vr->SetStackInfo(sinfo);
        sinfo->SuccWeight(vr->NowWeight());

        *imgr |= vr->Imgr();

        allocated = true;
    }

    if (not allocated) {
        auto mgr = std::make_unique<IntervalManager>();
        real_stk_inval_.push_back(std::move(mgr));

        auto sinfo = Alloca(vr->GetSize());
        real_stkinfo_.push_back(sinfo);

        vr->SetOnStack(true);
        vr->SetStackInfo(sinfo);
        sinfo->SuccWeight(vr->NowWeight());

        *mgr |= vr->Imgr();
    }
}

void RLPlanner::PlanStackSpace() {
    int64_t spoff = 0;

    spoff += 8 * param_stack_;

    std::priority_queue<StackInfo *> stkpq;
    std::queue<StackInfo *> allocas;

    for (auto &&sinfo : stk_storage_) {
        if (sinfo->IsFromAlloca()) {
            allocas.push(sinfo.get());
        }
        stkpq.push(sinfo.get());
    }

    while (not stkpq.empty()) {
        auto top = stkpq.top();
        stkpq.pop();

        auto siz = top->GetSLen();

        if (siz == 8) {
            spoff = RoundUp(8, spoff);
        }

        top->SetOff(spoff);
        spoff += siz;
    }

    while (not allocas.empty()) {
        auto stk = allocas.front();
        allocas.pop();

        auto siz = stk->GetSLen();

        stk->SetOff(spoff);
        spoff += siz;
    }

    int64_t idx = 0;
    idx -= 16;

    for (auto &&[rridx, imgr]: real_reg_inval_) {
        idx -= 8;
        place_to_save[rridx] = idx;
    }

    spoff += -idx;
    total_stack_size_ = spoff;
}