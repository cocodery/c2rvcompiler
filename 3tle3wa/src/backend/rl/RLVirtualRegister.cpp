#include "3tle3wa/backend/rl/RLVirtualRegister.hh"

#include "3tle3wa/backend/IntervalTree.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"

VirtualRegister::VirtualRegister(VREG_TYPE type, uint64_t vridx)
    : type_(type),
      vridx_(vridx),
      sinfo_(nullptr),
      ainfo_(nullptr),
      onstack_(false),
      real_regidx_(0),
      param_regidx_(0),
      assigned_(false),
      param_(false),
      retval_(false),
      this_ret_(false),
      imgr_(),
      saving_(false),
      save_off_(0) {}

void VirtualRegister::SetOnStack(bool on) { onstack_ = on; }

void VirtualRegister::SetRRidx(size_t rridx) {
    real_regidx_ = rridx;
    assigned_ = true;
}

void VirtualRegister::SetParam(size_t pos) {
    assigned_ = true;
    param_ = true;

    param_regidx_ = pos;
}

void VirtualRegister::SetRetval(bool on) { retval_ = on; }

void VirtualRegister::SetThisRet(bool on) { this_ret_ = on; }

void VirtualRegister::SetSaving(int64_t off) {
    save_off_ = off;
    saving_ = true;
}

int64_t VirtualRegister::SavingInfo() { return save_off_; }

bool VirtualRegister::IsSaving() { return saving_; }

bool VirtualRegister::IsAssigned() { return assigned_; }

bool VirtualRegister::IsParam() { return param_; }

bool VirtualRegister::IsRetval() { return retval_; }

bool VirtualRegister::IsThisRet() { return this_ret_; }

bool VirtualRegister::OnStk() { return onstack_; }

bool VirtualRegister::PtrOnStk() { return VREG_TYPE::PTR == type_ and onstack_; }

void VirtualRegister::SetStackInfo(StackInfo *sinfo) {
    if (sinfo_ != nullptr) {
        Assert(sinfo_->IsFromAlloca(), "ill formed");
        ainfo_ = sinfo_;
    }
    sinfo_ = sinfo;
}

StackInfo *VirtualRegister::GetStackInfo() { return sinfo_; }

StackInfo *VirtualRegister::GetAllocaInfo() {
    if (ainfo_ == nullptr) {
        return sinfo_;
    }
    return ainfo_;
}

VREG_TYPE VirtualRegister::GetType() { return type_; }

uint64_t VirtualRegister::GetVRIdx() const { return vridx_; }

size_t VirtualRegister::GetPPos() { return param_regidx_; }

size_t VirtualRegister::GetSize() {
    switch (type_) {
        case VREG_TYPE::FLT:
        case VREG_TYPE::INT:
            return sizeof(int);
        case VREG_TYPE::PTR:
            return sizeof(intptr_t);
    }
    return 0;
}

size_t VirtualRegister::GetRRid() {
    if (assigned_) {
        return real_regidx_;
    }
    panic("unexpected");
    return 0;
}

bool VirtualRegister::FGPR() const { return type_ == VREG_TYPE::FLT; }

void VirtualRegister::NewInterval(size_t idx, size_t begin, size_t end, size_t len) {
    imgr_.New(idx, len);
    imgr_.UpdateEnd(idx, begin, end);
}

void VirtualRegister::UpdateIntervalBegin(size_t idx, size_t begin, size_t len) {
    if (imgr_.AskInterval(idx) == nullptr) {
        imgr_.New(idx, len);
        imgr_.UpdateEnd(idx, begin, len);
    }
    imgr_.UpdateBegin(idx, begin);
}

void VirtualRegister::UpdateIntervalEnd(size_t idx, size_t begin, size_t end, size_t len) {
    if (imgr_.AskInterval(idx) == nullptr) {
        imgr_.New(idx, len);
    }
    imgr_.UpdateEnd(idx, begin, end);
}

IntervalManager &VirtualRegister::Imgr() { return imgr_; }

void VirtualRegister::CallAt(size_t lbidx, size_t pos) { imgr_.CallAt(lbidx, pos); }

void VirtualRegister::UseAt(size_t lbidx) { imgr_.Hit(lbidx); }

double VirtualRegister::NowWeight() { return weight_; }

bool VirtualRegister::operator>(const VirtualRegister &other) {
    if (weight_ > other.weight_) {
        return true;
    } else if (weight_ < other.weight_) {
        return false;
    }

    if (vridx_ > other.vridx_) {
        return true;
    } else if (vridx_ < other.vridx_) {
        return false;
    }

    return false;
}

bool VirtualRegister::operator==(const VirtualRegister &other) {
    return weight_ == other.weight_ and vridx_ == other.vridx_;
}

bool VirtualRegister::operator<(const VirtualRegister &other) {
    if (weight_ > other.weight_) {
        return false;
    } else if (weight_ < other.weight_) {
        return true;
    }

    if (vridx_ > other.vridx_) {
        return false;
    } else if (vridx_ < other.vridx_) {
        return true;
    }

    return false;
}