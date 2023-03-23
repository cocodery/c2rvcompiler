#include "constant.hh"

Constant::Constant(TypeID _tid, ConstType _value) : BaseValue(_tid), value(_value) {
    assert((_tid & (CONST | BOOL | INT | FLOAT)) & CONST);
}

void Constant::fixValue(TypeID _tid) {
    BaseTypePtr base_type = this->getBaseType(); 
    switch (base_type->getMaskedType(BOOL, INT, FLOAT)) {
        case BOOL:  convert<bool>();  break;
        case INT:   convert<int32_t>();   break;
        case FLOAT: convert<float>(); break;
        default:    assert(false);
    }
    base_type->resetType(_tid);
} 

std::shared_ptr<BaseValue> Constant::unaryOperate(const std::string &op) {
    BaseTypePtr base_type = this->getBaseType();
    
    TypeID _tid = base_type->getMaskedType(INT, FLOAT, CONST);

    ConstType _value;

    if (op == "-") {
        std::visit([&_value](auto &&arg) { _value = -arg; }, value);
    } else if (op == "!") {
        std::visit([&_value](auto &&arg) { _value = !arg; }, value);
    } else {
        assert(false);
    }

    return CreatePtr(_tid, _value);
}

std::shared_ptr<Constant> Constant::CreatePtr(TypeID _tid, ConstType _value) {
    return std::make_shared<Constant>(_tid, _value);
}

std::string Constant::toString() {
    BaseTypePtr base_type = this->getBaseType();
    base_type->checkType(BOOL | INT | FLOAT);
    std::stringstream ss;
    ss << base_type->toString();
    ss << " -> ";
    std::visit([&ss](auto &&arg) { ss << arg; }, value);
    return ss.str();
}

std::string Constant::tollvmIR() {
    BaseTypePtr base_type = this->getBaseType();
    base_type->checkType(BOOL | INT | FLOAT, CONST);
    std::stringstream ss;
    ss << base_type->tollvmIR() << ' ';

    std::visit([&ss](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, float>) {
            double double_value = arg;
            uint64_t uint64_value = reinterpret_cast<uint64_t &>(double_value);
            char buf[20];
            sprintf(buf, "0x%16lx", uint64_value);
            ss << buf;
        } else {
            ss << arg;
        }
    }, value);

    return ss.str();
}
