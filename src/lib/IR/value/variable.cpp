#include "variable.hh"

size_t Variable::var_idx = 1;

Variable::Variable(BaseTypePtr _type) 
    : idx(var_idx++), BaseValue(_type) {
}

void Variable::resetVarIdx() {
    this->var_idx = 1;
}

VariablePtr Variable::CreatePtr(BaseTypePtr _type) {
    return std::make_shared<Variable>(_type);
}

std::string Variable::tollvmIR() {
    std::stringstream ss;
    ss << "%Reg_" << this->idx;
    return ss.str();
}