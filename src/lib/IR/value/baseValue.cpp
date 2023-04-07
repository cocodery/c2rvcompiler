#include "baseValue.hh"

BaseValue::BaseValue(BaseTypePtr _type) 
    : base_type(_type) {
    // attr_type != VOID
    assert(!base_type->voidType());
}

BaseTypePtr BaseValue::getBaseType() {
    return this->base_type;
}

std::ostream &operator<<(std::ostream &os, BaseValuePtr value) {
    os << value->tollvmIR();
    return os;
}