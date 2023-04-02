#pragma once

#include "instruction.hh"

using CondCode = uint64_t;
constexpr CondCode COND_LTH = 0x0001; // less than
constexpr CondCode COND_LEQ = 0x0002; // less or equal
constexpr CondCode COND_EQU = 0x0003; // equal
constexpr CondCode COND_NEQ = 0x0004; // unequal

class ICmpInst;
using ICmpInstPtr = std::shared_ptr<ICmpInst>;

class ICmpInst : public Instruction {
private:
    BaseValuePtr result;
    CondCode     cond;
    BaseValuePtr lhs;
    BaseValuePtr rhs;
public:
    ICmpInst(BaseValuePtr, CondCode, BaseValuePtr, BaseValuePtr);
    ~ICmpInst() = default;

    static ICmpInstPtr CreatePtr(BaseValuePtr, CondCode, BaseValuePtr, BaseValuePtr);
    
    std::string tollvmIR();
};

class FCmpInst;
using FCmpInstPtr = std::shared_ptr<FCmpInst>;

class FCmpInst : public Instruction {
private:
    BaseValuePtr result;
    CondCode     cond;
    BaseValuePtr lhs;
    BaseValuePtr rhs;
public:
    FCmpInst(BaseValuePtr, CondCode, BaseValuePtr, BaseValuePtr);
    ~FCmpInst() = default;

    static FCmpInstPtr CreatePtr(BaseValuePtr, CondCode, BaseValuePtr, BaseValuePtr);
    
    std::string tollvmIR();
};
