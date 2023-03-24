#pragma once

#include "baseValue.hh"

// UnInitVar only exsit in
//  1 -> Global UnInitialized Variable
//  2 -> Function Argument
// and only have type in { INT, FLOAT }
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