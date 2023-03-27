#include "baseValue.hh"

BaseValue::BaseValue(TypeID _tid) {
    base_type = ScalarType::CreatePtr(_tid);
}

BaseValue::BaseValue(ListTypePtr btptr) {
    base_type = std::static_pointer_cast<BaseType>(btptr);
}

BaseTypePtr BaseValue::getBaseType() {
    return this->base_type;
}

std::ostream &operator<<(std::ostream &os, BaseValuePtr value) {
    os << value->tollvmIR();
    return os;
}