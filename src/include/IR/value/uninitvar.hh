#pragma once

#include "baseValue.hh"

class UnInitVar : public BaseValue {
public:
    UnInitVar(TypeID);
    UnInitVar(ListTypePtr);

    void fixValue(TypeID) { return; }

    static std::shared_ptr<UnInitVar> CreatePtr(TypeID);
    static std::shared_ptr<UnInitVar> CreatePtr(ListTypePtr);
    
    std::string toString();

    std::string tollvmIR();
};