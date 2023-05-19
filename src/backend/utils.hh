#pragma once

#include <cstdint>
#include <type_traits>

namespace backend {

template <uint64_t align = 3, typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr static inline auto ROUNDUP(T x) -> decltype(x) {
    uint64_t base = 1 << align;
    uint64_t mask = ~(base - 1);
    return (mask & x) + base;
}

}  // namespace backend