#pragma once

#include "baseValue.hh"

class GlobalValue;
using GlobalValuePtr = std::shared_ptr<GlobalValue>;

class GlobalValue : public BaseValue {
   private:
    size_t idx;
    BaseValuePtr init_value;

    static size_t glb_idx;

   public:
    GlobalValue(BaseTypePtr, BaseValuePtr);

    BaseValuePtr GetInitValue();

    bool IsGlobalValue() const;

    void FixValue(ATTR_TYPE);

    static GlobalValuePtr CreatePtr(BaseTypePtr, BaseValuePtr);

    std::string tollvmIR();
};
