#pragma once

#include <string>

#include "valueHeader.hh"

class BaseFunction;
using BaseFuncPtr = std::shared_ptr<BaseFunction>;

class BaseFunction {
   protected:
    ScalarTypePtr ret_type;
    std::string func_name;
    ParamList param_list;

   public:
    BaseFunction(ScalarTypePtr, std::string &, ParamList &);
    ~BaseFunction() = default;

    ScalarTypePtr GetReturnType();

    std::string &GetFuncName();

    ParamList &GetParamList();

    virtual std::string tollvmIR() = 0;
};