#include <cinttypes>
#include <queue>
#include <unordered_set>

#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/backend/utils.hh"
#include "3tle3wa/riscv/spec.hh"

struct VRElem {
    VirtualRegister *vr_;

    bool operator>(const VRElem &other) const { return *vr_ > *other.vr_; }
    bool operator==(const VRElem &other) const { return *vr_ == *other.vr_; }
    bool operator<(const VRElem &other) const { return *vr_ < *other.vr_; }
};

struct STKElem {
    StackInfo *stk_;

    bool operator>(const STKElem &other) const { return *stk_ > *other.stk_; }
    bool operator==(const STKElem &other) const { return *stk_ == *other.stk_; }
    bool operator<(const STKElem &other) const { return *stk_ < *other.stk_; }
};

void RLPlanner::PlanRegisters(size_t igpr[], size_t igprlen, size_t fgpr[], size_t fgprlen) {
    std::priority_queue<VRElem> vrpq;
    std::priority_queue<VRElem> spill;
    std::priority_queue<VRElem> spill2;

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
        vrpq.push(VRElem{.vr_ = vr.get()});
    }

    while (not vrpq.empty()) {
        size_t *agpr;
        size_t alen;

        auto top = vrpq.top().vr_;
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
            spill.push(VRElem{.vr_ = top});
        }
    }

    //
    // for now, no split
    //

    while (not spill.empty()) {
        auto top = spill.top().vr_;
        spill.pop();

        spill2.push(VRElem{.vr_ = top});
    }

    //
    // spill
    //

    Log("spill %" PRIu64, spill2.size());

    while (not spill2.empty()) {
        auto top = spill2.top().vr_;
        spill2.pop();

        spillOn(top);
    }

    Log("use stack %" PRIu64, real_stkinfo_.size());
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
        if ((real_stkinfo_[i]->GetSLen() != vr->GetSize()) or (vr->Imgr() && *imgr)) {
            continue;
        }

        auto sinfo = real_stkinfo_[i];

        vr->SetOnStack(true);
        vr->SetStackInfo(sinfo);
        sinfo->SuccWeight(vr->NowWeight());

        *imgr |= vr->Imgr();

        allocated = true;
        break;
    }

    if (not allocated) {
        auto mgr = std::make_unique<IntervalManager>();
        auto bptr = mgr.get();
        real_stk_inval_.push_back(std::move(mgr));

        auto sinfo = Alloca(vr->GetSize());
        real_stkinfo_.push_back(sinfo);

        vr->SetOnStack(true);
        vr->SetStackInfo(sinfo);
        sinfo->SuccWeight(vr->NowWeight());

        *bptr |= vr->Imgr();
    }
}

void RLPlanner::PlanStackSpace() {
    int64_t spoff = 0;

    spoff += 8 * param_stack_;

    std::priority_queue<STKElem> stkpq;
    std::queue<StackInfo *> allocas;

    for (auto &&sinfo : stk_storage_) {
        if (sinfo->IsFromAlloca()) {
            allocas.push(sinfo.get());
        }
        if (sinfo->IsParam()) {
            continue;
        }
        stkpq.push(STKElem{.stk_ = sinfo.get()});
    }

    while (not stkpq.empty()) {
        auto top = stkpq.top().stk_;
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

    for (auto &&[rridx, imgr] : real_reg_inval_) {
        idx -= 8;
        place_to_save[rridx] = idx;
    }

    spoff += -idx;
    total_stack_size_ = spoff;
}