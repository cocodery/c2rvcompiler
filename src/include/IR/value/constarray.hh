#pragma once

#include <vector>

#include "baseValue.hh"

class ConstArray : public BaseValue {
public:
    void fixValue(TypeID) { return; }

    std::string toString() { return ""; }
};