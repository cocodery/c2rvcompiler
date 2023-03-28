#include "variable.hh"

size_t Variable::var_idx = 1;

Variable::Variable(TypeID _tid) : idx(var_idx++), BaseValue(_tid) {
    assert(this->getBaseType()->checkType(BOOL | INT | FLOAT, VARIABLE));
}

Variable::Variable(ListTypePtr list_type) : idx(var_idx++), BaseValue(list_type) {
    // assert(list_type->checkType(INT | FLOAT, ARRAY));
    // have checked when create ListType
    assert(this->getBaseType()->checkType(VARIABLE));
}

void Variable::resetVarIdx() {
    this->var_idx = 1;
}

VariablePtr Variable::CreatePtr(TypeID _tid) {
    return std::make_shared<Variable>(_tid);
}

VariablePtr Variable::CreatePtr(ListTypePtr list_type) {
    return std::make_shared<Variable>(list_type);
}

std::string Variable::tollvmIR() {
    std::stringstream ss;
    ss << "%Reg_" << this->idx;
    return ss.str();
}