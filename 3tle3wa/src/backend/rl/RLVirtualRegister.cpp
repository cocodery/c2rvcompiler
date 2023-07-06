#include "3tle3wa/backend/rl/RLVirtualRegister.hh"

VirtualRegister::VirtualRegister(VREG_TYPE type, uint64_t vridx) : type_(type), vridx_(vridx) {
    sinfo_ = nullptr;
    onstack_ = false;

    assigned_ = false;
    real_regidx_ = 0;
}

void VirtualRegister::SetOnStack(bool on) { onstack_ = on; }

void VirtualRegister::SetStackInfo(StackInfo *sinfo) { sinfo_ = sinfo; }

void VirtualRegister::calculateWeight() { weight_ = 12; }

uint64_t VirtualRegister::GetVRIdx() const { return vridx_; }
