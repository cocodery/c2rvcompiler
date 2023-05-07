#include "exprFlod.hh"

// Constant do UnaryOperate
// must have type in { BOOL, INT32, FLOAT }
BaseValuePtr ExprFlod::UnaryOperate(const OpCode op, const ConstantPtr oprand) {
    ATTR_TYPE _type;
    ConstType _value;

    std::visit(
        [&_type, &_value, oper = op](auto &&arg) {
            switch (oper) {
                case OP_MINUS:
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, float>) {
                        _type = FLOAT;
                    } else {
                        _type = INT32;
                    }
                    _value = -arg;
                    return;
                case OP_NOT:
                    _type = BOOL;
                    _value = !arg;
                    return;
                default:
                    assert(false);
            }
        },
        oprand->GetValue());

    ScalarTypePtr _stype = (_type == INT32) ? type_const_int : (_type == FLOAT) ? type_const_float : type_const_bool;
    return Constant::CreatePtr(_stype, _value);
}

// Constant do UnaryOperate
// must have type in { BOOL, INT32, FLOAT }
BaseValuePtr ExprFlod::BinaryOperate(const OpCode op, const ConstantPtr lhs, const ConstantPtr rhs) {
    ATTR_TYPE _type;
    ConstType _value;

    std::visit(
        [&_type, &_value, oper = op](auto &&l, auto &&r) {
            using type_l = std::decay_t<decltype(l)>;
            using type_r = std::decay_t<decltype(r)>;

            constexpr bool returns_float = std::is_same_v<type_l, float> || std::is_same_v<type_r, float>;

            if constexpr (returns_float) {
                _type = FLOAT;
            } else {
                _type = INT32;
            }

            switch (oper) {
                case OP_ADD:
                    _value = l + r;
                    return;
                case OP_SUB:
                    _value = l - r;
                    return;
                case OP_MUL:
                    _value = l * r;
                    return;
                case OP_DIV:
                    _value = l / r;
                    return;
                case OP_REM:
                    if constexpr (returns_float) {
                        assert(false);
                    } else {
                        _value = l % r;
                    }
                    return;
                case OP_LTH:
                    _value = (l < r);
                    _type = BOOL;
                    return;
                case OP_LEQ:
                    _value = (l <= r);
                    _type = BOOL;
                    return;
                case OP_GTH:
                    _value = (l > r);
                    _type = BOOL;
                    return;
                case OP_GEQ:
                    _value = (l >= r);
                    _type = BOOL;
                    return;
                case OP_EQU:
                    _value = (l == r);
                    _type = BOOL;
                    return;
                case OP_NEQ:
                    _value = (l != r);
                    _type = BOOL;
                    return;
                default:
                    assert(false);
            }
        },
        lhs->GetValue(), rhs->GetValue());

    ScalarTypePtr _stype = (_type == INT32) ? type_const_int : (_type == FLOAT) ? type_const_float : type_const_bool;

    return Constant::CreatePtr(_stype, _value);
}