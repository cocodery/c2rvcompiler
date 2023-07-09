#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <variant>

#define SEXT32(val, prev)               \
    do {                                \
        struct {                        \
            int32_t v : prev;           \
        } val##__sext__ = {.v = val};   \
        val = (int32_t)val##__sext__.v; \
    } while (0)

/**
 * @brief 判断立即数 imm 是否能够用宽度为 width bit 的补码表示
 *
 * @param width 指定宽度
 * @param imm 立即数
 * @return true
 * @return false
 */
static inline constexpr bool ImmWithin(size_t width, int64_t imm) {
    uint64_t max_ = (1ul << (width - 1)) - 1;
    uint64_t min_ = ~(1ul << (width - 1)) + 1;
    return (int64_t)min_ <= imm and imm <= (int64_t)max_;
}

static_assert(ImmWithin(12, 0b011111111111));
static_assert(ImmWithin(12, 0b000000000000));
static_assert(ImmWithin(12, -0b100000000000));
static_assert(ImmWithin(12, 0b100000000000) == false);

/**
 * @brief 不小于 num 的 align 的最小倍数
 *
 * @param align
 * @param num
 * @return constexpr uint64_t
 */
static inline constexpr uint64_t RoundUp(uint64_t align, uint64_t num) {
    uint64_t integer = num / align;
    uint64_t rest = num % align;
    return (integer + !!(rest)) * align;
}

static_assert(RoundUp(16, 0) != 16);
static_assert(RoundUp(16, 16) == 16);
static_assert(RoundUp(16, 23) == 32);
static_assert(RoundUp(16, 48) == 48);

/**
 * @brief C 形式的 reinterpretation
 */
union Reinterp32Bit {
    float flt_;
    uint32_t u32_;
    int32_t i32_;
};

/**
 * @brief C 形式的 reinterpretation
 */
union Reinterp64Bit {
    double dlb_;
    uint64_t u64_;
    int64_t i64_;
};

/**
 * @brief 常数展开信息
 *
 */
struct ConstValueInfo {
    size_t width_{0};
    bool isflt_{false};
    bool illegel_{false};

    Reinterp32Bit v32_{0};
    Reinterp64Bit v64_{0};
};

using ConstPackage = std::variant<bool, char, int32_t, float, int64_t>;

/**
 * @brief 展开 llvm ir 常量指针
 *
 * @param cstptr 常量指针
 * @return cval_info 常量展开信息
 */
static inline ConstValueInfo XConstValue(const ConstPackage &cpk) {
    ConstValueInfo cvalinfo;

    std::visit(
        [&cvalinfo](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, float>) {
                cvalinfo.v32_.flt_ = arg;
                cvalinfo.isflt_ = true;
                cvalinfo.width_ = 32;
            } else if constexpr (std::is_same_v<T, double>) {
                cvalinfo.v32_.flt_ = arg;
                cvalinfo.v64_.dlb_ = arg;
                cvalinfo.isflt_ = true;
                cvalinfo.width_ = 64;
            } else if constexpr (std::is_same_v<T, int32_t>) {
                cvalinfo.v32_.i32_ = arg;
                cvalinfo.isflt_ = false;
                cvalinfo.width_ = 32;
            } else if constexpr (std::is_same_v<T, bool>) {
                cvalinfo.v32_.i32_ = arg;
                cvalinfo.isflt_ = false;
                cvalinfo.width_ = 32;
            } else if constexpr (std::is_same_v<T, char>) {
                cvalinfo.v32_.i32_ = arg;
                cvalinfo.isflt_ = false;
                cvalinfo.width_ = 32;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                cvalinfo.v64_.i64_ = arg;
                cvalinfo.isflt_ = false;
                cvalinfo.width_ = 64;
            } else {
                // mean illegel
                cvalinfo.illegel_ = true;
            }
        },
        cpk);

    return cvalinfo;
}

