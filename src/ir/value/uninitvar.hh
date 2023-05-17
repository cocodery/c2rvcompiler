#pragma once

#include "baseValue.hh"

class UnInitVar;
using UnInitVarPtr = std::shared_ptr<UnInitVar>;

// UnInitVar only exsit in Global UnInitialized Variable
// and only have type in { INT32, FLOAT }
class UnInitVar : public BaseValue {
   public:
    UnInitVar(BaseTypePtr);

    bool IsUnInitVar() const;

    static UnInitVarPtr CreatePtr(BaseTypePtr);

    std::string tollvmIR();
};