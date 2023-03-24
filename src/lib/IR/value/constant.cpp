#include "constant.hh"

Constant::Constant(TypeID _tid, ConstType _value) : BaseValue(_tid), value(_value) {
    this->getBaseType()->checkType(INT | FLOAT, CONSTANT);
}

ConstType &Constant::getValue() {
    return this->value;
}

void Constant::fixValue(TypeID _tid) {
    BaseTypePtr base_type = this->getBaseType();
    switch (_tid & (BOOL | INT | FLOAT)) {
        case BOOL:  convert<bool>();    break;
        case INT:   convert<int32_t>(); break;
        case FLOAT: convert<float>();   break;
        default:    assert(false);
    }
    // _tid has checked above
    // append CONSTANT
    base_type->resetType(_tid | CONSTANT);
    // no need to checkType
} 

// Constant who do unaryOperate
// must have type in { BOOL, INT, FLOAT }
std::shared_ptr<BaseValue> Constant::unaryOperate(const std::string &op) {
    BaseTypePtr base_type = getBaseType();
    base_type->checkType(BOOL | INT | FLOAT);
    
    // when return this value to a global constant, will do fixValue
    TypeID _tid = base_type->getMaskedType(BOOL | INT | FLOAT, CONSTANT);

    ConstType _value;
    if (op == "-") {
        assert(!base_type->BoolType()); // omit single minus to bool type value
        std::visit([&_value](auto &&arg) { _value = -arg; }, value);
        return CreatePtr(_tid, _value);
    } else if (op == "!") {
        std::visit([&_value](auto &&arg) { _value = !arg; }, value);
        return CreatePtr(BOOL | CONSTANT, _value);
    } else {
        assert(false);
    }
}

// Constant who do unaryOperate
// must have type in { INT, FLOAT }
std::shared_ptr<BaseValue> Constant::binaryOperate(const std::string &op, const std::shared_ptr<Constant> rhs) {
    this->getBaseType()->checkType(BOOL | INT | FLOAT);
    rhs ->getBaseType()->checkType(BOOL | INT | FLOAT);
    // if (op == "+") {
    //     return std::make_shared<Constant>(*this + *rhs);
    // } else if (op == "-") {
    //     return std::make_shared<Constant>(*this - *rhs);
    // } else if (op == "*") {
    //     return std::make_shared<Constant>(*this * *rhs);
    // } else if (op == "/") {
    //     return std::make_shared<Constant>(*this / *rhs);
    // } else if (op == "%") {
    //     return std::make_shared<Constant>(*this % *rhs);
    // } 
    assert(0);
}

// Constant *Constant::operator+(Constant rhs) {

// }

// Constant *Constant::operator-(Constant rhs) {

// }

// Constant *Constant::operator*(Constant rhs) {

// }

// Constant *Constant::operator/(Constant rhs) {

// }

// Constant *Constant::operator%(Constant rhs) {

// }


std::shared_ptr<Constant> Constant::CreatePtr(TypeID _tid, ConstType _value) {
    return std::make_shared<Constant>(_tid, _value);
}

std::string Constant::toString() {
    BaseTypePtr base_type = this->getBaseType();
    base_type->checkType(INT | FLOAT, CONSTANT);

    std::stringstream ss;
    ss << base_type->toString();
    ss << " -> ";
    std::visit([&ss](auto &&arg) { ss << arg; }, value);

    return ss.str();
}

std::string Constant::tollvmIR() {
    BaseTypePtr base_type = this->getBaseType();
    base_type->checkType(INT | FLOAT, CONSTANT);

    std::stringstream ss;
    ss << base_type->tollvmIR() << ' ';
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
