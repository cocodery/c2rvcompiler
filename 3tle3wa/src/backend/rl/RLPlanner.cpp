#include "3tle3wa/backend/rl/RLPlanner.hh"

#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"

RLPlanner::RLPlanner(size_t regidx) : stkidx_(0), regidx_(regidx), param_stack_(0) {}

StackInfo *RLPlanner::Alloca(size_t len) {
    stkidx_ += 1;

    auto stkinfo = std::make_unique<StackInfo>(stkidx_, len);
    auto bptr = stkinfo.get();

    CRVC_UNUSE auto result = sinfo_map_.emplace(stkidx_, bptr);
    Assert(result.second, "stkinfo add failed");

    stk_storage_.push_back(std::move(stkinfo));
    return bptr;
}

VirtualRegister *RLPlanner::Alloca(uint64_t vridx, size_t len) {
    auto ptr = AllocVReg(VREG_TYPE::PTR, vridx);
    auto stk = Alloca(len);

    stk->SetFromAlloca(true);
    ptr->SetStackInfo(stk);

    return ptr;
}

VirtualRegister *RLPlanner::AllocParam(VREG_TYPE type, uint64_t vridx, size_t len, bool onstk, size_t pos) {
    auto ptr = AllocVReg(type, vridx);
    if (onstk) {
        auto stk = Alloca(len);
        stk->SetParam(pos * 8);
        ptr->SetOnStack(true);
        ptr->SetStackInfo(stk);
        ptr->SetParam(pos);
    } else {
        ptr->SetOnStack(false);
        ptr->SetParam(pos);
    }
    return ptr;
}

VirtualRegister *RLPlanner::AllocVReg(VREG_TYPE type, uint64_t vridx) {
    if (auto fnd = vr_map_.find(vridx); fnd != vr_map_.end()) {
        return fnd->second;
    }

    auto vreg = std::make_unique<VirtualRegister>(type, vridx);
    auto bptr = vreg.get();

    CRVC_UNUSE auto result = vr_map_.emplace(vridx, bptr);
    Assert(result.second, "virtual register add failed");

    vr_storage_.push_back(std::move(vreg));

    return bptr;
}

VirtualRegister *RLPlanner::NewVReg(VREG_TYPE type) {
    regidx_ += 1;

    auto vreg = std::make_unique<VirtualRegister>(type, regidx_);
    auto bptr = vreg.get();

    CRVC_UNUSE auto result = vr_map_.emplace(regidx_, bptr);
    Assert(result.second, "virtual register add failed");

    vr_storage_.push_back(std::move(vreg));

    return bptr;
}

VirtualRegister *RLPlanner::GetVReg(uint64_t vridx) {
    // avoid format
    return vr_map_.at(vridx);
}

void RLPlanner::Link(uint64_t income, uint64_t old) {
    // avoid format
    vr_map_[income] = vr_map_.at(old);
}

void RLPlanner::SetPstkSiz(size_t sps) { param_stack_ = std::max(param_stack_, sps); }
