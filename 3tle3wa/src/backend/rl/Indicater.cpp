#include "3tle3wa/backend/rl/Indicater.hh"

void LiveInterval::UseThis() { UseTime += 1; }

void LiveInterval::CallThis(size_t pos) {
    CallTime[pos] += 1;
    CallVec |= (1 << pos);
}

bool LiveInterval::operator()(size_t point) const { return point >= Begin && point < End; }

bool LiveInterval::operator^(const LiveInterval &other) const {
    if (other.Begin > End or other.End <= Begin) {
        return false;
    }
    return true;
}