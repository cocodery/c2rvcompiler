#include "constant.hh"

Constant::Constant(TypeID _tid, ConstType _value) : BaseValue(_tid), value(_value) {
    assert(this->getBaseType()->checkType(INT | FLOAT, CONSTANT));
}

ConstType &Constant::getValue() {
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

// Constant who do unaryOperate
// must have type in { BOOL, INT, FLOAT }
std::shared_ptr<BaseValue> Constant::unaryOperate(const std::string &op) {
    assert(this->getBaseType()->checkType(BOOL | INT | FLOAT));
    
    // when return this value to a global constant, will do fixValue
    TypeID _tid;
    ConstType _value;

    std::visit([&_tid, &_value, oper = op[0]](auto &&arg) {
        switch (oper) {
            case '-':
                if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, float>) {
                    _tid = FLOAT | CONSTANT;
                } else {
                    _tid = INT | CONSTANT;
                }
                _value = -arg;
                return;
            case '!':
                _tid = BOOL | CONSTANT;
                _value = !arg;
                return;
            default:
                assert(false);
        }
    }, value);

    return CreatePtr(_tid, _value);
}

// Constant who do unaryOperate
// must have type in { INT, FLOAT }
std::shared_ptr<BaseValue> Constant::binaryOperate(const std::string &op, const std::shared_ptr<Constant> rhs) {
    assert(this->getBaseType()->checkType(BOOL | INT | FLOAT));
    assert(rhs ->getBaseType()->checkType(BOOL | INT | FLOAT));

    TypeID _tid;
    ConstType _value;

    std::visit([&_tid, &_value, oper = op[0]](auto &&l, auto &&r) {
        using type_l = std::decay_t<decltype(l)>;
        using type_r = std::decay_t<decltype(r)>;

        constexpr bool returns_float = std::is_same_v<type_l, float> || std::is_same_v<type_r, float>;

        if constexpr (returns_float) {
            _tid = FLOAT | CONSTANT;
        } else {
            _tid = INT | CONSTANT;
        }

        switch (oper) {
            case '+': _value = l + r; return;
            case '-': _value = l - r; return;
            case '*': _value = l * r; return;
            case '/': _value = l / r; return;
            case '%':
                if constexpr (returns_float) {
                    assert(false);
                } else {
                    _value = l % r;
                }
                return;
            default: assert(false);
        }
    }, value, rhs->value);

    return CreatePtr(_tid, _value);
}

std::shared_ptr<Constant> Constant::CreatePtr(TypeID _tid, ConstType _value) {
    return std::make_shared<Constant>(_tid, _value);
}

std::string Constant::toString() {
    BaseTypePtr base_type = this->getBaseType();
    assert(base_type->checkType(INT | FLOAT, CONSTANT));

    std::stringstream ss;
    ss << base_type->toString();
    ss << " -> ";
    std::visit([&ss](auto &&arg) { ss << arg; }, value);

    return ss.str();
}

std::string Constant::tollvmIR() {
    BaseTypePtr base_type = this->getBaseType();
    assert(base_type->checkType(INT | FLOAT, CONSTANT));

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
