#pragma once

#include <vector>

#include "../valueHeader.hh"
#include "basicblock.hh"

using Parameter = std::pair<std::string, BaseValuePtr>;
using ParamList = std::vector<Parameter>;

class Function {
private:
    ScalarTypePtr ret_type;
    std::string func_name;
    ParamList param_list;
    BlockPtr block; // point to first block of function
public:
    Function(ScalarTypePtr, std::string &, ParamList &);
    ~Function() = default;

    ParamList &getParamList();

    void setBlock(BlockPtr);

    static std::shared_ptr<Function> CreatePtr(ScalarTypePtr, std::string &, ParamList &);

    std::string toString();
};

using FunctionPtr = std::shared_ptr<Function>;