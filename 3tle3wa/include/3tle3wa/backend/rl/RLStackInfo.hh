#pragma once

#include <cstddef>
#include <cstdint>

class StackInfo {
    size_t stack_len_;

    int64_t offset_;

    bool allocated_;

   public:
    void SetOff(int64_t off) const;
    int64_t GetOff() const;

    void SetSLen(size_t slen) const;
    size_t GetSLen() const;
};