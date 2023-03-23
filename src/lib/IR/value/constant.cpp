#include "constant.hh"

Constant::Constant(TypeID _tid, ConstType _value) : BaseValue(_tid), value(_value) {
    assert((_tid & (CONST | BOOL | INT | FLOAT)) & CONST);
}

void Constant::fixValue(TypeID _tid) {
    BaseTypePtr base_type = this->getBaseType(); 
    switch (this->getBaseType()->getMaskedType(BOOL, INT, FLOAT)) {
        case BOOL:  convert<bool>();  break;
        case INT:   convert<int>();   break;
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

    std::stringstream ss;

    ss << base_type->tollvmIR() << ' ';

    if (base_type->BoolType()) {
        ss << this->getValue<bool>();
    } else if (base_type->IntType()) {
        ss << this->getValue<int32_t>();
    } else if (base_type->FloatType()) {
        double float_value = double(this->getValue<float>());
        uint64_t uint64_value = reinterpret_cast<uint64_t &>(float_value);
        ss << "0x" << std::hex << std::setw(16) << std::setfill('0') << uint64_value;
    } else {
        assert(0);
    }

    return ss.str();
}
