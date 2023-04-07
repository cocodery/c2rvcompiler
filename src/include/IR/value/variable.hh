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

    void fixValue(ATTR_TYPE) { return; }

    static void resetVarIdx();

    static VariablePtr CreatePtr(BaseTypePtr);

    std::string tollvmIR();
};