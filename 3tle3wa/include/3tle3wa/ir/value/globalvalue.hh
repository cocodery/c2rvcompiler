#pragma once

#include "3tle3wa/ir/value/baseValue.hh"

class GlobalValue;
using GlobalValuePtr = std::shared_ptr<GlobalValue>;

class GlobalValue final : public BaseValue {
   private:
    size_t idx;
    BaseValuePtr init_value;

    static size_t glb_idx;

   public:
    GlobalValue(BaseTypePtr, BaseValuePtr);

    void SetInitValue(BaseValuePtr);
    BaseValuePtr GetInitValue() const;
    size_t GetGlobalValueIdx() const;

    bool IsGlobalValue() const;

    static GlobalValuePtr CreatePtr(BaseTypePtr, BaseValuePtr);

    std::string tollvmIR();
};
