#include "valueHeader.hh"

// Constant do unaryOperate
// must have type in { BOOL, INT, FLOAT }
BaseValuePtr Value::unaryOperate(const OpCode op, const ConstantPtr oprand) {
    ATTR_TYPE _type;
    ConstType _value;

    std::visit(
        [&_type, &_value, oper = op](auto &&arg) {
            switch (oper) {
                case OP_MINUS:
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, float>) {
                        _type = FLOAT;
                    } else {
                        _type = INT;
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
        oprand->getValue());

    ScalarTypePtr _stype = (_type == INT) ? type_const_int : (_type == FLOAT) ? type_const_float : type_const_bool;
    return Constant::CreatePtr(_stype, _value);
}

// Constant do unaryOperate
// must have type in { BOOL, INT, FLOAT }
BaseValuePtr Value::binaryOperate(const OpCode op, const ConstantPtr lhs, const ConstantPtr rhs) {
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
                _type = INT;
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
        lhs->getValue(), rhs->getValue());

    ScalarTypePtr _stype = (_type == INT) ? type_const_int : (_type == FLOAT) ? type_const_float : type_const_bool;

    return Constant::CreatePtr(_stype, _value);
}

BaseValuePtr Value::unaryOperate(const OpCode op, BaseValuePtr oprand, CfgNodePtr block) {
    assert(oprand->IsOprand());
    if (op == OP_ADD) return oprand;
    if (oprand->IsConstant()) {
        return unaryOperate(op, std::static_pointer_cast<Constant>(oprand));
    } else {
        ATTR_TYPE _type = oprand->getBaseType()->getAttrType();
        ConstantPtr zero = (_type == BOOL) ? zero_bool : ((_type == INT) ? zero_int32 : zero_float);
        if (op == OP_MINUS) {
            return binaryOperate(OP_SUB, zero, oprand, block);
        } else if (op == OP_NOT) {
            if (_type == FLOAT) {
                return FCmpInst::DoFCompare(OP_EQU, oprand, zero, block);
            } else {
                return ICmpInst::DoICompare(OP_EQU, oprand, zero, block);
            }
        }
    }
    assert(0);
}

BaseValuePtr Value::binaryOperate(const OpCode op, BaseValuePtr lhs, BaseValuePtr rhs, CfgNodePtr block) {
    assert(lhs->IsOprand() && rhs->IsOprand());
    if (lhs->IsConstant() && rhs->IsConstant()) {
        return binaryOperate(op, std::static_pointer_cast<Constant>(lhs), std::static_pointer_cast<Constant>(rhs));
    }
    BaseValuePtr i_lhs = lhs, f_lhs = lhs;
    BaseValuePtr i_rhs = rhs, f_rhs = rhs;

    ATTR_TYPE lhs_type = lhs->getBaseType()->getAttrType();
    ATTR_TYPE rhs_type = rhs->getBaseType()->getAttrType();

    if ((op & (OP_ADD | OP_SUB | OP_MUL | OP_DIV | OP_REM | OP_LSHIFT | OP_RSHIFT)) != 0) {
        // when do arithmetic operation, lhs_type == rhs_type in { INT, FLOAT }
        if ((lhs_type != rhs_type) || ((lhs_type & rhs_type) == BOOL)) {
            if (lhs_type == FLOAT || rhs_type == FLOAT) {
                f_lhs = scalarTypeConvert(FLOAT, f_lhs, block);
                f_rhs = scalarTypeConvert(FLOAT, f_rhs, block);
                lhs_type = rhs_type = FLOAT;
            } else {
                i_lhs = scalarTypeConvert(INT, i_lhs, block);
                i_rhs = scalarTypeConvert(INT, i_rhs, block);
                lhs_type = rhs_type = INT;
            }
        }
        assert(lhs_type == rhs_type);
        if (lhs_type == INT) {
            return IBinaryInst::DoIBinOperate(op, i_lhs, i_rhs, block);
        } else {
            return FBinaryInst::DoFBinOperate(op, f_lhs, f_rhs, block);
        }
    } else if ((op & (OP_LTH | OP_LEQ | OP_GTH | OP_GEQ | OP_EQU | OP_NEQ)) != 0) {
        // when do compare operation, lhs_type == rhs_type in { BOOL, INT, FLOAT }
        if (lhs_type != rhs_type) {
            if (lhs_type == BOOL || rhs_type == BOOL) {  // if one is BOOL, convert the other to BOOL
                i_lhs = scalarTypeConvert(BOOL, i_lhs, block);
                i_rhs = scalarTypeConvert(BOOL, i_rhs, block);
                lhs_type = rhs_type = BOOL;
            } else if (lhs_type == FLOAT || rhs_type == FLOAT) {
                f_lhs = scalarTypeConvert(FLOAT, f_lhs, block);
                f_rhs = scalarTypeConvert(FLOAT, f_rhs, block);
                lhs_type = rhs_type = FLOAT;
            }  // else, Both type are INT
        }
        assert(lhs_type == rhs_type);
        if (lhs_type == FLOAT) {
            return FCmpInst::DoFCompare(op, f_lhs, f_rhs, block);
        } else {
            return ICmpInst::DoICompare(op, i_lhs, i_rhs, block);
        }
    }
    assert(0);
}

BaseValuePtr Value::scalarTypeConvert(ATTR_TYPE type_convert, BaseValuePtr convertee, CfgNodePtr block) {
    assert(convertee->IsOprand());
    ATTR_TYPE type_convertee = convertee->getBaseType()->getAttrType();
    // if type_convert == type_convertee, no need to convert
    if (type_convert == type_convertee) {
        return convertee;
    }
    // if convertee is `CONSTANT`, use `fixType` to convert
    if (convertee->IsConstant()) {
        ConstantPtr constant_convertee = std::static_pointer_cast<Constant>(convertee);
        ScalarTypePtr _stype = (type_convertee == INT)     ? type_const_int
                               : (type_convertee == FLOAT) ? type_const_float
                                                           : type_const_bool;
        ConstantPtr constant = Constant::CreatePtr(_stype, constant_convertee->getValue());
        constant->fixValue(type_convert);
        return constant;
    }
    // use instruction to convert
    if (type_convert == FLOAT) {
        // convert i1 or i32 to float
        return SitoFpInst::DoSitoFp(convertee, block);
    } else if (type_convert == INT) {
        if (type_convertee == FLOAT) {
            // convert float to i32 or i1
            return FptoSiInst::DoFptoSi(type_convert, convertee, block);
        } else if (type_convertee == BOOL) {
            // convert i1 to i32
            return ZextInst::DoZeroExt(convertee, block);
        }
    } else {
        if (type_convertee == INT) {
            // convert i32 to i1
            return ICmpInst::DoICompare(OP_NEQ, convertee, zero_int32, block);
        } else if (type_convertee == FLOAT) {
            // convert float to i1
            return FCmpInst::DoFCompare(OP_NEQ, convertee, zero_float, block);
        }
    }
    assert(0);
}
