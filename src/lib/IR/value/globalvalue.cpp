#include "globalvalue.hh"

size_t GlobalValue::glb_idx = 1;

GlobalValue::GlobalValue(BaseTypePtr _type, BaseValuePtr _value) 
    : BaseValue(_type), idx(glb_idx++), init_value(_value) {
    // INT || FLOAT
    assert(base_type->intType() || base_type->floatType());
    // GLOBAL
    assert(base_type->IsGlobal());
}

BaseValuePtr GlobalValue::getInitValue() {
    return this->init_value;
}

GlobalValuePtr GlobalValue::CreatePtr(BaseTypePtr _type, BaseValuePtr _value) {
    return std::make_shared<GlobalValue>(_type, _value);
}

void GlobalValue::fixValue(ATTR_TYPE _type) {
    this->init_value->fixValue(_type);
} 

std::string GlobalValue::tollvmIR() {
    std::stringstream ss;
    ss << "@Glb_" << this->idx;
    return ss.str();
}
