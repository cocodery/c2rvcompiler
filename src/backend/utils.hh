#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "../ir/IR.hh"

static inline constexpr bool imm_width_within(uint64_t width, uint64_t imm) {
    uint64_t max_pos = (1 << (width - 1)) - 1;
    uint64_t max_neg = ~(1 << (width - 1)) + 1;
    return (int64_t)max_neg <= (int64_t)imm && (int64_t)imm <= (int64_t)max_pos;
}

static_assert(imm_width_within(12, 0b011111111111));
static_assert(imm_width_within(12, 0b000000000000));
static_assert(imm_width_within(12, -0b100000000000));

static inline constexpr uint64_t ROUNDUP(uint64_t size, uint64_t num) {
    uint64_t integer = num / size;
    uint64_t rest = num % size;
    return (integer + !!(rest)) * size;
}

static_assert(ROUNDUP(16, 16) == 16);
static_assert(ROUNDUP(16, 23) == 32);
static_assert(ROUNDUP(16, 48) == 48);

struct cstpack {
    bool isfloat;
    uint64_t value;
};

static inline cstpack extractcst(ConstantPtr &cstptr) {
    cstpack pk{false, 0};
    std::visit(
        [&pk](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, float>) {
                double double_value = arg;
                float float_value = double_value;
                pk.value = reinterpret_cast<uint32_t &>(float_value);
                pk.isfloat = true;
            } else {
                pk.value = static_cast<int64_t>(arg);
                pk.isfloat = false;
            }
        },
        cstptr->GetValue());
    return pk;
}
