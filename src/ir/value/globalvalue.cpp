#include "globalvalue.hh"

size_t GlobalValue::glb_idx = 1;

GlobalValue::GlobalValue(BaseTypePtr _type, BaseValuePtr _value)
    : BaseValue(_type), idx(glb_idx++), init_value(_value) {
    // INT32 || FLOAT
    if (base_type->GetAttrType() != init_value->GetBaseType()->GetAttrType()) {
        init_value->FixValue(base_type->GetAttrType());
    }
    // GLOBAL, POINTER
    assert(base_type->IsGlobal() && base_type->IsPointer());
    assert(base_type->GetAttrScalar() == init_value->GetBaseType()->GetAttrScalar());
}

BaseValuePtr GlobalValue::getInitValue() { return this->init_value; }

bool GlobalValue::IsGlobalValue() const { return true; }

GlobalValuePtr GlobalValue::CreatePtr(BaseTypePtr _type, BaseValuePtr _value) {
    return std::make_shared<GlobalValue>(_type, _value);
}

void GlobalValue::FixValue(ATTR_TYPE _type) { this->init_value->FixValue(_type); }

std::string GlobalValue::tollvmIR() {
    std::stringstream ss;
    ss << "@Glb_" << this->idx;
    return ss.str();
}
