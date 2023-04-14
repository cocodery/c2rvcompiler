#include "constant.hh"

Constant::Constant(ScalarTypePtr _type, ConstType _value) 
    : BaseValue(_type), value(_value) {
    // BOOL || INT || FLOAT
    assert(base_type->boolType() || base_type->intType() || base_type->floatType());
    // Immutable, NotPtr, Scalar
    assert(base_type->IsImMutable() && base_type->IsNotPtr() && base_type->IsScalar());
}

const ConstType &Constant::getValue() const {
    return this->value;
}

void Constant::fixValue(ATTR_TYPE _type) {
    std::visit([&_value = this->value, _type](auto &&arg) {
        switch (_type) {
            case BOOL:  _value = static_cast<bool>(arg);    return;
            case INT:   _value = static_cast<int32_t>(arg); return;
            case FLOAT: _value = static_cast<float>(arg);   return;
            default:    assert(false);
        }
    }, value);
    // for Constant, 
    // its base_type come from ScalarType::{ type_const_bool, type_const_int, type_const_float }
    // if directly modify their ATTR_TYPE, will distory later use
    // so replace original type with an allocated one 
    base_type = (_type == INT)  ? type_const_int :
                (_type == FLOAT)? type_const_float :
                                  type_const_bool
                                ;
} 

ConstantPtr Constant::CreatePtr(ScalarTypePtr _type, ConstType _value) {
    return std::make_shared<Constant>(_type, _value);
}

std::string Constant::tollvmIR() {
    std::stringstream ss;

    BaseTypePtr base_type = this->getBaseType();
    std::visit([&ss, base_type](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, float>) {
            assert(base_type->floatType());
            double double_value = arg;
            uint64_t uint64_value = reinterpret_cast<uint64_t &>(double_value);
            char buf[20];
            sprintf(buf, "0x%016lx", uint64_value);
            ss << buf;
        } else {
            ss << arg;
        }
    }, value);

    return ss.str();
}
