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

    BaseValuePtr getInitValue();

    static GlobalValuePtr CreatePtr(BaseTypePtr, BaseValuePtr);

    bool IsConstant() { return false; };

    void fixValue(ATTR_TYPE);

    std::string tollvmIR();
};
