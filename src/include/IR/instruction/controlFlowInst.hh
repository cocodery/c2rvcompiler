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

class CallInst;
using CallInstPtr = std::shared_ptr<CallInst>;
using RParamList = std::vector<BaseValuePtr>;

class CallInst : public Instruction {
private:
    ScalarTypePtr ret_type;
    BaseValuePtr ret_value;
    std::string callee_name;
    RParamList rparam_list;
public:
    CallInst(ScalarTypePtr, BaseValuePtr, std::string &, RParamList &);
    ~CallInst() = default;

    static CallInstPtr CreatePtr(ScalarTypePtr, BaseValuePtr, std::string &, RParamList &);

    std::string tollvmIR();
};
