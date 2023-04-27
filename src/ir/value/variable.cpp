#include "variable.hh"

size_t Variable::var_idx = 1;

Variable::Variable(BaseTypePtr _type) : BaseValue(_type), idx(var_idx++) {
    // attr_type != VOID, Mutable
    assert(!base_type->VoidType() && base_type->IsMutable());
    // PARAMETER or LOCAL
    assert(base_type->IsParameter() || base_type->IsLocal());
}

void Variable::ResetVarIdx() { var_idx = 1; }
size_t Variable::GetVarIdx() { return var_idx; }
void Variable::SetVarIdx(size_t _var_idx) { var_idx = _var_idx; }

VariablePtr Variable::CreatePtr(BaseTypePtr _type) { return std::make_shared<Variable>(_type); }

std::string Variable::tollvmIR() {
    std::stringstream ss;
    ss << "%Reg_" << this->idx;
    return ss.str();
}