#include "globalvalue.hh"

size_t GlobalValue::glb_idx = 1;

GlobalValue::GlobalValue(BaseTypePtr _type, BaseValuePtr _value)
    : BaseValue(_type), idx(glb_idx++), init_value(_value) {
    assert(init_value->IsConstant() || init_value->IsConstArray() || init_value->IsUnInitVar());
    // GLOBAL, POINTER
    assert(base_type->IsGlobal() && base_type->IsPointer());
    assert(base_type->GetAttrScalar() == init_value->GetBaseType()->GetAttrScalar());
}

void GlobalValue::SetInitValue(BaseValuePtr value) {
    assert(value->IsConstant() || value->IsConstArray() || value->IsUnInitVar());
    init_value = value;
}

BaseValuePtr GlobalValue::GetInitValue() const { return this->init_value; }

bool GlobalValue::IsGlobalValue() const { return true; }

GlobalValuePtr GlobalValue::CreatePtr(BaseTypePtr _type, BaseValuePtr _value) {
    return std::make_shared<GlobalValue>(_type, _value);
}

std::string GlobalValue::tollvmIR() {
    std::stringstream ss;
    ss << "@Glb_" << this->idx;
    return ss.str();
}
