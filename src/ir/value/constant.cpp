#include "constant.hh"

Constant::Constant(ScalarTypePtr _type, ConstType _value) : BaseValue(_type), value(_value) {
    // NONE-VOID
    assert(!base_type->VoidType());
    // Immutable, NotPtr, Scalar
    assert(base_type->IsImMutable() && base_type->IsNotPtr() && base_type->IsScalar());
}

const ConstType &Constant::GetValue() const { return this->value; }

bool Constant::IsConstant() { return true; }

void Constant::FixValue(ATTR_TYPE _type) {
    std::visit(
        [&_value = this->value, _type](auto &&arg) {
            switch (_type) {
                case BOOL:
                    _value = static_cast<bool>(arg);
                    return;
                case INT32:
                    _value = static_cast<int32_t>(arg);
                    return;
                case FLOAT:
                    _value = static_cast<float>(arg);
                    return;
                default:
                    assert(false);
            }
        },
        value);
    // for Constant,
    // its base_type come from ScalarType::{ type_const_bool, type_const_int, type_const_float }
    // if directly modify their ATTR_TYPE, will distory later use
    // so replace original type with an allocated one
    base_type = (_type == INT32) ? type_const_int : (_type == FLOAT) ? type_const_float : type_const_bool;
}

ConstantPtr Constant::CreatePtr(ScalarTypePtr _type, ConstType _value) {
    return std::make_shared<Constant>(_type, _value);
}

std::string Constant::tollvmIR() {
    std::stringstream ss;

    BaseTypePtr base_type = this->GetBaseType();
    std::visit(
        [&ss, base_type](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, float>) {
                assert(base_type->FloatType());
                double double_value = arg;
                uint64_t uint64_value = reinterpret_cast<uint64_t &>(double_value);
                char buf[20];
                sprintf(buf, "0x%016lx", uint64_value);
                ss << buf;
            } else {
                ss << arg;
            }
        },
        value);

    return ss.str();
}
