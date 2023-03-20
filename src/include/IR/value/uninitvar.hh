#pragma once

#include "baseValue.hh"

class UnInitVar : public BaseValue {
public:
    void fixValue(TypeID) { return; }
    
    std::string toString() { return ""; }
};