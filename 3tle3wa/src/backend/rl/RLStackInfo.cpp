#include "3tle3wa/backend/rl/RLStackInfo.hh"

StackInfo::StackInfo(size_t sidx, size_t slen)
    : sidx_(sidx), stack_len_(slen), offset_(0), succ_weight_(0.0), from_alloca_(false), param_(false) {}

size_t StackInfo::GetSidx() const { return sidx_; }

size_t StackInfo::GetSLen() const { return stack_len_; }

void StackInfo::SetOff(int64_t off) { offset_ = off; }

void StackInfo::SetParam(int64_t off) {
    SetOff(off);
    param_ = true;
}

bool StackInfo::IsParam() { return param_; }

int64_t StackInfo::GetOff() const { return offset_; }

void StackInfo::SetFromAlloca(bool from_alloca) { from_alloca_ = from_alloca; }

bool StackInfo::IsFromAlloca() { return from_alloca_; }

void StackInfo::CalcuWeight() {
    double standard = 100.0;

    standard = standard * succ_weight_ + standard / (stack_len_ * 25 + 1);

    weight_ = standard;
}

void StackInfo::SuccWeight(double w) { weight_ += w; }

bool StackInfo::operator>(const StackInfo &other) {
    if (weight_ > other.weight_) {
        return true;
    } else if (weight_ < other.weight_) {
        return false;
    }

    if (sidx_ > other.sidx_) {
        return true;
    } else if (sidx_ < other.sidx_) {
        return false;
    }

    return false;
}

bool StackInfo::operator==(const StackInfo &other) {
    return weight_ == other.weight_ and sidx_ == other.sidx_;
}

bool StackInfo::operator<(const StackInfo &other) {
    if (weight_ > other.weight_) {
        return false;
    } else if (weight_ < other.weight_) {
        return true;
    }

    if (sidx_ > other.sidx_) {
        return false;
    } else if (sidx_ < other.sidx_) {
        return true;
    }

    return false;
}