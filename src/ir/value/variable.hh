#pragma once

#include "baseValue.hh"

class Variable;
using VariablePtr = std::shared_ptr<Variable>;

class Variable : public BaseValue {
   private:
    size_t idx;

    static size_t var_idx;

   public:
    Variable(BaseTypePtr);
    ~Variable() = default;

    bool IsConstant() { return false; };

    void fixValue(ATTR_TYPE) { return; }

    static void ResetVarIdx();
    static size_t GetVarIdx();
    static void SetVarIdx(size_t);

    static VariablePtr CreatePtr(BaseTypePtr);

    std::string tollvmIR();
};