#pragma once

#include "baseValue.hh"

class Variable;
using VariablePtr = std::shared_ptr<Variable>;

class Variable : public BaseValue {
private:
    size_t idx;

    static size_t var_idx;
public:
    Variable(TypeID);
    Variable(ListTypePtr);
    ~Variable() = default;

    void fixValue(TypeID) { return; }

    void resetVarIdx();

    static VariablePtr CreatePtr(TypeID);
    static VariablePtr CreatePtr(ListTypePtr);
    
    std::string toString() { return ""; }

    std::string tollvmIR();
};