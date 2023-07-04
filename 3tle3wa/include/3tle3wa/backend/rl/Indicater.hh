#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

struct AtCall {
    size_t CallIdx{0};
    size_t AtParamPos{0};

    bool operator>(const AtCall &other) { return CallIdx > other.CallIdx; }
    bool operator==(const AtCall &other) { return CallIdx == other.CallIdx; }
    bool operator<(const AtCall &other) { return CallIdx < other.CallIdx; }
};

struct LiveInterval {
    size_t LableIdx;
    size_t Begin;
    size_t End;
};

struct VRIndicator {
    double StaticLoop{0.0};
    double HotLoop{0.0};

    uint64_t LiveLength{0};
    uint64_t ActiveTime{0};
    uint64_t SliceNum{0};

    std::vector<AtCall> Calls;
};