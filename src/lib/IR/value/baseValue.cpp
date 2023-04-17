#include "baseValue.hh"

BaseValue::BaseValue(BaseTypePtr _type) 
    : base_type(_type) {
    // attr_type != VOID
    assert(!base_type->VoidType());
}

BaseTypePtr BaseValue::getBaseType() {
    return this->base_type;
}

bool BaseValue::IsOprand() {
    return (!base_type->VoidType() && base_type->IsNotPtr() && base_type->IsScalar());
}

std::ostream &operator<<(std::ostream &os, BaseValuePtr value) {
    os << value->tollvmIR();
    return os;
}