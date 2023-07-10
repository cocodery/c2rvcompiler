#pragma once

#include <cstddef>
#include <cstdint>

#include "3tle3wa/backend/Interface.hh"

class StackInfo final : public Weightable {
    size_t sidx_;

    size_t stack_len_;

    int64_t offset_;

    double succ_weight_;

    bool from_alloca_;

    bool param_;

   public:
    StackInfo(size_t sidx, size_t slen);

    void SetParam(int64_t off);

    void CalcuWeight();

    void SuccWeight(double w);

    bool IsParam();

    size_t GetSidx() const;

    size_t GetSLen() const;

    void SetOff(int64_t off);

    void SetFromAlloca(bool from_alloca);

    bool IsFromAlloca();

    int64_t GetOff() const;

    virtual bool operator>(const StackInfo &other) final;

    virtual bool operator==(const StackInfo &other) final;

    virtual bool operator<(const StackInfo &other) final;
};