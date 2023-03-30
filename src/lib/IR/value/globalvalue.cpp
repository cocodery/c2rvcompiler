#include "globalvalue.hh"

size_t GlobalValue::glb_idx = 1;

GlobalValue::GlobalValue(BaseTypePtr _type, BaseValuePtr _value) 
    : idx(glb_idx++), init_value(_value), BaseValue(_type) {
}

BaseValuePtr GlobalValue::getInitValue() {
    return this->init_value;
}

GlobalValuePtr GlobalValue::CreatePtr(BaseTypePtr _type, BaseValuePtr _value) {
    return std::make_shared<GlobalValue>(_type, _value);
}

void GlobalValue::fixValue(TypeID _tid) {
    this->init_value->fixValue(_tid);
} 

std::string GlobalValue::tollvmIR() {
    std::stringstream ss;
    ss << "@Glb_" << this->idx;
    return ss.str();
}
