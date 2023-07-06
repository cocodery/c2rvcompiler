#pragma once

#include <cstddef>
#include <cstdint>

class StackInfo {
    size_t sidx_;

    size_t stack_len_;

    int64_t offset_;

    bool allocated_;

   public:
    StackInfo(size_t sidx, size_t slen);

    size_t GetSidx() const;

    size_t GetSLen() const;

    void SetOff(int64_t off);
    int64_t GetOff() const;
};