#pragma once

#include "baseValue.hh"

class Variable : public BaseValue {
public:
    void fixValue(TypeID) { return; }
    
    std::string toString() { return ""; }

    std::string tollvmIR() { return ""; }
};