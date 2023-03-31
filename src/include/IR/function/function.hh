#pragma once

#include <vector>

#include "../valueHeader.hh"
#include "basicblock.hh"

class Function;
using Parameter = std::pair<std::string, BaseValuePtr>;
using ParamList = std::vector<Parameter>;
using FunctionPtr = std::shared_ptr<Function>;

class Function {
private:
    ScalarTypePtr ret_type;
    std::string func_name;
    ParamList param_list;
    BlockPtr block; // point to first block of function
public:
    Function(ScalarTypePtr, std::string &, ParamList &, BlockPtr);
    ~Function() = default;

    ScalarTypePtr getReturnType();

    ParamList &getParamList();

    static FunctionPtr CreatePtr(ScalarTypePtr, std::string &, ParamList &, BlockPtr);

    std::string toString();

    std::string tollvmIR();
};

std::ostream &operator<<(std::ostream &, FunctionPtr);
