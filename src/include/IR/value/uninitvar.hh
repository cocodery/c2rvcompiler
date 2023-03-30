#pragma once

#include "baseValue.hh"

class UnInitVar;
using UnInitVarPtr = std::shared_ptr<UnInitVar>;

// UnInitVar only exsit in Global UnInitialized Variable
// and only have type in { INT, FLOAT }
class UnInitVar : public BaseValue {
public:
    UnInitVar(BaseTypePtr);

    void fixValue(TypeID) { return; }
    
    static UnInitVarPtr CreatePtr(BaseTypePtr);
    
    std::string toString();

    std::string tollvmIR();
};