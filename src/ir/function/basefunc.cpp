#include "basefunc.hh"

//===-----------------------------------------------------------===//
//                     BaseFunction Implementation
//===-----------------------------------------------------------===//

BaseFunction::BaseFunction(ScalarTypePtr _type, std::string &_name, ParamList &_list, bool _side_effect)
    : ret_type(_type), func_name(_name), param_list(_list), side_effect(_side_effect), recursive(false) {}

bool BaseFunction::GetSideEffect() const { return side_effect; }
void BaseFunction::SetSideEffect(bool _bool) { side_effect = _bool; }

bool BaseFunction::GetRecursive() const { return recursive; }
void BaseFunction::SetRecursive(bool _bool) { recursive = _bool; }

void BaseFunction::InsertCallWho(BaseFuncPtr func) { call_who.insert(func); }
CallerFunction &BaseFunction::GetCallWho() { return call_who; }

void BaseFunction::InsertWhoCall(BaseFuncPtr func) { who_call.insert(func); }
CalleeFunction &BaseFunction::GetWhoCall() { return who_call; }

bool BaseFunction::IsBeUsed() const { return (who_call.size() > 0) || (func_name == "main"); }

ScalarTypePtr BaseFunction::GetReturnType() { return this->ret_type; }

std::string &BaseFunction::GetFuncName() { return this->func_name; }

ParamList &BaseFunction::GetParamList() { return this->param_list; }