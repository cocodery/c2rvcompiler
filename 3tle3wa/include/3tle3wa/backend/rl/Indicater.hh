#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

// [Begin, End)
struct LiveInterval {
    size_t LableIdx;
    size_t Begin;
    size_t End;

    size_t CallTime[64] = {0};
    size_t CallVec = 0;

    size_t UseTime = 0;

    void UseThis();

    void CallThis(size_t pos);

    bool operator()(size_t point) const;

    bool operator^(const LiveInterval &other) const;
};