#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <variant>

/**
 * @brief 判断立即数 imm 是否能够用宽度为 width bit 的补码表示
 *
 * @param width 指定宽度
 * @param imm 立即数
 * @return true
 * @return false
 */
static inline constexpr bool imm_within(size_t width, int64_t imm) {
    uint64_t max_ = (1 << (width - 1)) - 1;
    uint64_t min_ = ~(1 << (width - 1)) + 1;
    return (int64_t)min_ <= imm and imm <= (int64_t)max_;
}

static_assert(imm_within(12, 0b011111111111));
static_assert(imm_within(12, 0b000000000000));
static_assert(imm_within(12, -0b100000000000));

/**
 * @brief 不小于 num 的 align 的最小倍数
 *
 * @param align
 * @param num
 * @return constexpr uint64_t
 */
static inline constexpr uint64_t round_up(uint64_t align, uint64_t num) {
    uint64_t integer = num / align;
    uint64_t rest = num % align;
    return (integer + !!(rest)) * align;
}

static_assert(round_up(16, 16) == 16);
static_assert(round_up(16, 23) == 32);
static_assert(round_up(16, 48) == 48);

/**
 * @brief 常数展开信息
 *
 */
struct cval_info {
    bool is_w64;
    bool is_flt;

    uint32_t v32{0};
    uint64_t v64{0};
};

using cstpack_t = std::variant<bool, char, int32_t, float, int64_t>;

/**
 * @brief 展开 llvm ir 常量指针
 *
 * @param cstptr 常量指针
 * @return cval_info 常量展开信息
 */
static inline cval_info xcval(const cstpack_t &cpk) {
    cval_info cval{false, false};
    std::visit(
        [&cval](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, float>) {
                float float_value = arg;
                cval.v32 = reinterpret_cast<uint32_t &>(float_value);
                cval.is_flt = true;
                cval.is_w64 = false;
            } else if constexpr (std::is_same_v<T, double>) {
                double double_value = arg;
                float float_value = double_value;
                cval.v32 = reinterpret_cast<const uint32_t &>(float_value);
                cval.is_flt = true;
                cval.is_w64 = false;
            } else if constexpr (std::is_same_v<T, int32_t>) {
                cval.v32 = reinterpret_cast<const uint32_t &>(arg);
                cval.is_flt = false;
                cval.is_w64 = false;
            } else if constexpr (std::is_same_v<T, bool>) {
                cval.v32 = reinterpret_cast<const bool &>(arg);
                cval.is_flt = false;
                cval.is_w64 = false;
            } else if constexpr (std::is_same_v<T, char>) {
                cval.v32 = reinterpret_cast<const char &>(arg);
                cval.is_flt = false;
                cval.is_w64 = false;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                cval.v64 = reinterpret_cast<const uint64_t &>(arg);
                cval.is_flt = false;
                cval.is_w64 = true;
            } else {
                // mean illegel
                cval.v32 = -1;
                cval.v64 = -1;
                cval.is_flt = true;
                cval.is_w64 = true;
            }
        },
        cpk);
    return cval;
}

static inline std::string gen_pblk_label(size_t lbidx) {
    // prefix L => label
    return ".L" + std::to_string(lbidx);
}

static inline std::string gen_loc_cst_label(size_t value) {
    // prefix LC => local constant
    // for float

    char buf[16] = {0};
    sprintf(buf,
            ".LC"
            "%08lx",
            value);
    return buf;
}

static inline std::string gen_glb_val_label(size_t idx) {
    // prefix GV => global value

    return ".GV" + std::to_string(idx);
}
