#include "3tle3wa/backend/rl/RLStackInfo.hh"

StackInfo::StackInfo(size_t sidx, size_t slen) : sidx_(sidx), stack_len_(slen) { offset_ = 0; }

size_t StackInfo::GetSidx() const { return sidx_; }

size_t StackInfo::GetSLen() const { return stack_len_; }

void StackInfo::SetOff(int64_t off) { offset_ = off; }

int64_t StackInfo::GetOff() const { return offset_; }
