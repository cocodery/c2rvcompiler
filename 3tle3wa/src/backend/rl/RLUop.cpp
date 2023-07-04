#include "3tle3wa/backend/rl/RLUop.hh"

size_t UopGeneral::GetUopIdx() const { return uop_idx_; }

void UopGeneral::SetUopIdx(size_t idx) { uop_idx_ = idx; }

COMP_KIND UopICmp::GetKind() const { return kind_; }

COMP_KIND UopFCmp::GetKind() const { return kind_; }

IBIN_KIND UopIBin::GetKind() const { return kind_; }

IBIN_KIND UopIBinImm::GetKind() const { return kind_; }

FBIN_KIND UopFBin::GetKind() const { return kind_; }