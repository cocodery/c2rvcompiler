#pragma once

#include <cstdint>
#include <vector>

#include "3tle3wa/riscv/spec.hh"

constexpr double MUL_LATENCY = 3;
constexpr double DIVREM_LATENCY = 3 * 0.7 + 68 * 0.3;
constexpr double LOAD_HIT_LANTENCY = 3;

constexpr double FLOAT_LATENCY = 4;

constexpr uint64_t HOLD_ACCMEM = 1;
constexpr uint64_t HOLD_BRJ = 1;
constexpr uint64_t HOLD_FLT = 1;
constexpr uint64_t HOLD_MDR = 1;

struct Transaction {
    std::vector<uint64_t> resource_required_;
    uint64_t resource_occupied_;
    std::vector<int64_t> other_info_;
    riscv::OpType optype_;
};
