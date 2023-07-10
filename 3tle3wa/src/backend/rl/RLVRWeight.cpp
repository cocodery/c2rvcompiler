#include "3tle3wa/backend/IntervalTree.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"

static constexpr double initial = 10.0;
static constexpr double hit_weight = 15.0;

static constexpr double blk_num_weight = 50.0;
static constexpr double live_span_weight = 1.5;

static double calcu(double hit, double blk, double live_span) {
    double result = initial;
    result += hit * hit_weight;
    result /= blk_num_weight * (blk - 1) + live_span * live_span_weight;

    return result;
}

void VirtualRegister::CalcuWeight() { weight_ = calcu(imgr_.TotalHit(), imgr_.Num(), imgr_.TotalLiveSpan()); }

double VirtualRegister::CalcuBlkWeight(size_t lbidx) {
    return calcu(imgr_.BlkHit(lbidx), imgr_.Num(), imgr_.BlkLiveSpan(lbidx));
}