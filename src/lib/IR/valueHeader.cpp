#include "valueHeader.hh"

bool Value::isConstant(BaseValuePtr value) {
    BaseTypePtr base_type = value->getBaseType();
    return (!base_type->voidType() && base_type->IsImMutable() && base_type->IsNotPtr() && base_type->IsScalar());
}

bool Value::bothConstant(BaseValuePtr lhs, BaseValuePtr rhs) {
    return isConstant(lhs) && isConstant(rhs);
}

// Constant do unaryOperate
// must have type in { BOOL, INT, FLOAT }
BaseValuePtr Value::unaryOperate(const OpCode op, const ConstantPtr oprand) {
    ATTR_TYPE _type;
    ConstType _value;

    std::visit([&_type, &_value, oper = op](auto &&arg) {
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
    }, oprand->getValue());

    return Constant::CreatePtr(ScalarType::CreatePtr(_type, IMMUTABLE, NOTPTR, SCALAR, NONE4), _value);
}

// Constant do unaryOperate
// must have type in { BOOL, INT, FLOAT }
BaseValuePtr Value::binaryOperate(const OpCode op, const ConstantPtr lhs, const ConstantPtr rhs) {
    ATTR_TYPE _type;
    ConstType _value;

    std::visit([&_type, &_value, oper = op](auto &&l, auto &&r) {
        using type_l = std::decay_t<decltype(l)>;
        using type_r = std::decay_t<decltype(r)>;

        constexpr bool returns_float = std::is_same_v<type_l, float> || std::is_same_v<type_r, float>;

        if constexpr (returns_float) {
            _type = FLOAT;
        } else {
            _type = INT;
        }

        switch (oper) {
            case OP_ADD: _value = l + r; return;
            case OP_SUB: _value = l - r; return;
            case OP_MUL: _value = l * r; return;
            case OP_DIV: _value = l / r; return;
            case OP_REM:
                if constexpr (returns_float) {
                    assert(false);
                } else {
                    _value = l % r;
                }
                return;
            case OP_LTH: _value = (l <  r); _type = BOOL; return;
            case OP_LEQ: _value = (l <= r); _type = BOOL; return;
            case OP_GTH: _value = (l >  r); _type = BOOL; return;
            case OP_GEQ: _value = (l >= r); _type = BOOL; return;
            case OP_EQU: _value = (l == r); _type = BOOL; return;
            case OP_NEQ: _value = (l != r); _type = BOOL; return;
            default: assert(false);
        }
    }, lhs->getValue(), rhs->getValue());

    return Constant::CreatePtr(ScalarType::CreatePtr(_type, IMMUTABLE, NOTPTR, SCALAR, NONE4), _value);
}

BaseValuePtr Value::unaryOperate(const OpCode op, BaseValuePtr oprand, BlockPtr block) {
    assert(oprand->getBaseType()->IsScalar());
    if (oprand->getBaseType()->IsPointer()) {
        oprand = LoadInst::DoLoadValue(oprand, block);
    }
    if (op == OP_ADD) return oprand;

    if (isConstant(oprand)) {
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

BaseValuePtr Value::binaryOperate(const OpCode op, BaseValuePtr lhs, BaseValuePtr rhs, BlockPtr block) {
    if (lhs->getBaseType()->IsPointer()) {
        lhs = LoadInst::DoLoadValue(lhs, block);
    }
    if (rhs->getBaseType()->IsPointer()) {
        rhs = LoadInst::DoLoadValue(rhs, block);
    }
    if (bothConstant(lhs, rhs)) {
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
            if (lhs_type == BOOL || rhs_type == BOOL) { // if one is BOOL, convert the other to BOOL
                i_lhs = scalarTypeConvert(BOOL, i_lhs, block);
                i_rhs = scalarTypeConvert(BOOL, i_rhs, block);
                lhs_type = rhs_type = BOOL;
            } else if (lhs_type == FLOAT || rhs_type == FLOAT) {
                f_lhs = scalarTypeConvert(FLOAT, f_lhs, block);
                f_rhs = scalarTypeConvert(FLOAT, f_rhs, block);
                lhs_type = rhs_type = FLOAT;
            } // else, Both type are INT
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

BaseValuePtr Value::scalarTypeConvert(ATTR_TYPE type_convert, BaseValuePtr convertee, BlockPtr block) {
    // target type must in `BOOL` or `INT` or `FLOAT`
    assert((type_convert == BOOL) || (type_convert == INT) || (type_convert == FLOAT));
    // convertee should be a Scalar
    assert(convertee->getBaseType()->IsScalar());
    if (convertee->getBaseType()->IsPointer()) {
        convertee = LoadInst::DoLoadValue(convertee, block);
    }
    ATTR_TYPE type_convertee = convertee->getBaseType()->getAttrType();
    // if type_convert == type_convertee, no need to convert
    if (type_convert == type_convertee) {
        return convertee;
    }
    // if convertee is `CONSTANT`, use `fixType` to convert
    if (isConstant(convertee)) {
        ConstantPtr constant_convertee = std::static_pointer_cast<Constant>(convertee);
        ConstantPtr constant = Constant::CreatePtr(ScalarType::CreatePtr(type_convertee, IMMUTABLE, NOTPTR, SCALAR, NONE4), constant_convertee->getValue());
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
