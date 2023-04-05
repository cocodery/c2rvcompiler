#include "constant.hh"

Constant::Constant(ScalarTypePtr _type, ConstType _value) : BaseValue(_type), value(_value) {
    assert(this->getBaseType()->checkType(BOOL | INT | FLOAT, CONSTANT)); 
    fixValue(_type->getMaskedType(BOOL | INT | FLOAT));
}

const ConstType &Constant::getValue() const {
    return this->value;
}

void Constant::fixValue(TypeID _tid) {
    std::visit([&_value = this->value, tid = _tid & (BOOL | INT | FLOAT)](auto &&arg) {
        switch (tid) {
            case BOOL:  _value = static_cast<bool>(arg);    return;
            case INT:   _value = static_cast<int32_t>(arg); return;
            case FLOAT: _value = static_cast<float>(arg);   return;
            default:    assert(false);
        }
    }, value);
    // _tid has checked above
    // append CONSTANT
    this->getBaseType()->resetType(_tid | CONSTANT);
    // no need to checkType
} 

ConstantPtr Constant::CreatePtr(ScalarTypePtr _type, ConstType _value) {
    return std::make_shared<Constant>(_type, _value);
}

std::string Constant::tollvmIR() {
    BaseTypePtr base_type = this->getBaseType();
    assert(base_type->checkType(BOOL | INT | FLOAT, CONSTANT));

    std::stringstream ss;
    std::visit([&ss](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, float>) {
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
