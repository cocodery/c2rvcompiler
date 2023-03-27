#pragma once

#include "baseValue.hh"

class UnInitVar;
using UnInitVarPtr = std::shared_ptr<UnInitVar>;

// UnInitVar only exsit in
//  1 -> Global UnInitialized Variable
//  2 -> Function Argument
// and only have type in { INT, FLOAT }
class UnInitVar : public BaseValue {
public:
    UnInitVar(TypeID);
    UnInitVar(ListTypePtr);

    void fixValue(TypeID) { return; }

    static UnInitVarPtr CreatePtr(TypeID);
    static UnInitVarPtr CreatePtr(ListTypePtr);
    
    std::string toString();

    std::string tollvmIR();
};