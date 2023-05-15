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

template <typename StorageType>
class UDGraph {
   public:
    class Edge {
       public:
        static size_t another(size_t self, size_t edge) { return self xor edge; }
        static size_t connect(size_t lhs, size_t rhs) { return lhs xor rhs; };
    };

    class Vertex {
        size_t idx_;
        StorageType value_;
        std::vector<size_t> edges_;

       public:
        Vertex(size_t idx, StorageType value) : idx_(idx), value_(value), edges_() {}
        auto GetIDX() const -> size_t { return idx_; }
        auto GetValue() -> StorageType& { return value_; }
        auto GetEdges() -> std::vector<size_t>& { return edges_; }
    };

   private:
    std::vector<Vertex> ps_;
    std::unordered_map<StorageType, size_t> map_;
};

}  // namespace backend