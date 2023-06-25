#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>

using rid_t = size_t;

using i32 = int32_t;
using u32 = uint32_t;

using i64 = int64_t;
using u64 = uint64_t;

using cstr = const char *;

class asm_inst {
   public:
    virtual std::string_view to_string() = 0;
    virtual std::string_view comment() = 0;
    virtual void set_comment(const char *comt) = 0;
    virtual ~asm_inst() = default;
};