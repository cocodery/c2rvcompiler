#include "baseValue.hh"

BaseValue::BaseValue(TypeID _tid) {
    base_type = std::make_shared<ScalarType>(_tid);
}

BaseValue::BaseValue(ListTypePtr btptr) {
    base_type = btptr;
}

BaseTypePtr BaseValue::getBaseType() {
    return this->base_type;
}