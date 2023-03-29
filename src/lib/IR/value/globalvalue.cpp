#include "globalvalue.hh"

size_t GlobalValue::glb_idx = 1;

GlobalValue::GlobalValue(BaseTypePtr _type, BaseValuePtr _value) 
    : idx(glb_idx++), init_value(_value), BaseValue(_type) {
}

GlobalValuePtr GlobalValue::CreatePtr(BaseTypePtr _type, BaseValuePtr _value) {
    return std::make_shared<GlobalValue>(_type, _value);
}

std::string GlobalValue::tollvmIR() {
    std::stringstream ss;
    ss << "@Glb_" << this->idx;
    return ss.str();
}
