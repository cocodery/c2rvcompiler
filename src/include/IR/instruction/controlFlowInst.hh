#pragma once

#include "instruction.hh"
#include "value/type/scalarType.hh"

class ReturnInst;
using RetInstPtr = std::shared_ptr<ReturnInst>;

class ReturnInst : public Instruction {
private:
    ScalarTypePtr ret_type;
    BaseValuePtr  ret_value;
public:
    ReturnInst(ScalarTypePtr, BaseValuePtr);
    ~ReturnInst() = default;

    static RetInstPtr CreatePtr(ScalarTypePtr, BaseValuePtr);

    std::string tollvmIR();
};