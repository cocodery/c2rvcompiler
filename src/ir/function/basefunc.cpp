#include "basefunc.hh"

//===-----------------------------------------------------------===//
//                     BaseFunction Implementation
//===-----------------------------------------------------------===//

BaseFunction::BaseFunction(ScalarTypePtr _type, std::string &_name, ParamList &_list)
    : ret_type(_type), func_name(_name), param_list(_list) {}

ScalarTypePtr BaseFunction::GetReturnType() { return this->ret_type; }

std::string &BaseFunction::GetFuncName() { return this->func_name; }

ParamList &BaseFunction::GetParamList() { return this->param_list; }