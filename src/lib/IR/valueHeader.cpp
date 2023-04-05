#include "valueHeader.hh"

bool Value::isConstant(BaseValuePtr value) {
    BaseTypePtr base_type = value->getBaseType();
    return (base_type->ConstantType() && !base_type->ArrayType());
}

bool Value::bothConstant(BaseValuePtr value1, BaseValuePtr value2) {
    return isConstant(value1) && isConstant(value2);
}


// Constant who do unaryOperate
// must have type in { BOOL, INT, FLOAT }
BaseValuePtr Value::unaryOperate(const OpCode op, const ConstantPtr oprand) {
    assert(oprand->getBaseType()->checkType(BOOL | INT | FLOAT));
    
    // when return this value to a global constant, will do fixValue
    TypeID _tid;
    ConstType _value;

    std::visit([&_tid, &_value, oper = op](auto &&arg) {
        switch (oper) {
            case OP_MINUS:
                if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, float>) {
                    _tid = FLOAT | CONSTANT;
                } else {
                    _tid = INT | CONSTANT;
                }
                _value = -arg;
                return;
            case OP_NOT:
                _tid = BOOL | CONSTANT;
                _value = !arg;
                return;
            default:
                assert(false);
        }
    }, oprand->getValue());

    return Constant::CreatePtr(ScalarType::CreatePtr(_tid), _value);
}

// Constant who do unaryOperate
// must have type in { INT, FLOAT }
BaseValuePtr Value::binaryOperate(const OpCode op, const ConstantPtr lhs, const ConstantPtr rhs) {
    assert(lhs->getBaseType()->checkType(BOOL | INT | FLOAT, CONSTANT));
    assert(rhs->getBaseType()->checkType(BOOL | INT | FLOAT, CONSTANT));

    TypeID _tid;
    ConstType _value;

    std::visit([&_tid, &_value, oper = op](auto &&l, auto &&r) {
        using type_l = std::decay_t<decltype(l)>;
        using type_r = std::decay_t<decltype(r)>;

        constexpr bool returns_float = std::is_same_v<type_l, float> || std::is_same_v<type_r, float>;

        if constexpr (returns_float) {
            _tid = FLOAT | CONSTANT;
        } else {
            _tid = INT | CONSTANT;
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
            case OP_LTH: _value = (l <  r); _tid = BOOL | CONSTANT; return;
            case OP_LEQ: _value = (l <= r); _tid = BOOL | CONSTANT; return;
            case OP_GTH: _value = (l >  r); _tid = BOOL | CONSTANT; return;
            case OP_GEQ: _value = (l >= r); _tid = BOOL | CONSTANT; return;
            case OP_EQU: _value = (l == r); _tid = BOOL | CONSTANT; return;
            case OP_NEQ: _value = (l != r); _tid = BOOL | CONSTANT; return;
            default: assert(false);
        }
    }, lhs->getValue(), rhs->getValue());

    return Constant::CreatePtr(ScalarType::CreatePtr(_tid), _value);
}

BaseValuePtr Value::unaryOperate(const OpCode op, BaseValuePtr value, BlockPtr block) {
    if (value->getBaseType()->checkType(POINTER)) {
        value = LoadInst::LoadValuefromMem(value, block);
    }
    if (op == OP_ADD) return value;

    if (isConstant(value)) {
        ConstantPtr constant = std::dynamic_pointer_cast<Constant>(value);
        return unaryOperate(op, constant);
    } else {
        TypeID id_type = value->getBaseType()->getMaskedType(BOOL | INT | FLOAT);
        if (op == OP_MINUS) {
            // value may be any type of { BOOL, INT, FLOAT }
            // when bool do single minus, binaryOperate trans to 2 integer
            ConstantPtr zero =  (id_type == BOOL) ? zero_bool  :
                                (id_type == INT)  ? zero_int32 :
                                                    zero_float;
            return binaryOperate(OP_SUB, zero, value, block);
        } else if (op == OP_NOT) {
            BaseValuePtr value_bool = Variable::CreatePtr(ScalarType::CreatePtr(BOOL | VARIABLE));
            if (id_type == INT) {
                InstPtr icmp_inst = ICmpInst::CreatePtr(value_bool, OP_EQU, value, zero_int32);
                block->insertInst(icmp_inst);
            } else if (id_type == FLOAT) {
                InstPtr fcmp_inst = FCmpInst::CreatePtr(value_bool, OP_EQU, value, zero_float);
                block->insertInst(fcmp_inst);
            } else {
                InstPtr icmp_inst = ICmpInst::CreatePtr(value_bool, OP_EQU, value, zero_bool);
                block->insertInst(icmp_inst);
            }
            return value_bool;
        }
    }
    assert(0);
}

BaseValuePtr Value::binaryOperate(const OpCode op, BaseValuePtr lhs, BaseValuePtr rhs, BlockPtr block) {
    if (lhs->getBaseType()->checkType(POINTER)) {
        lhs = LoadInst::LoadValuefromMem(lhs, block);
    }
    if (rhs->getBaseType()->checkType(POINTER)) {
        rhs = LoadInst::LoadValuefromMem(rhs, block);
    }
    if (bothConstant(lhs, rhs)) {
        ConstantPtr constant1 = std::dynamic_pointer_cast<Constant>(lhs);
        ConstantPtr constant2 = std::dynamic_pointer_cast<Constant>(rhs);
        return binaryOperate(op, constant1, constant2);
    } 

    BaseValuePtr i_lhs = lhs, f_lhs = lhs;
    BaseValuePtr i_rhs = rhs, f_rhs = rhs;

    TypeID lhs_type = lhs->getBaseType()->getMaskedType(BOOL | INT | FLOAT);
    TypeID rhs_type = rhs->getBaseType()->getMaskedType(BOOL | INT | FLOAT);

    if ((op & (OP_ADD | OP_SUB | OP_MUL | OP_DIV | OP_REM | OP_LSHIFT | OP_RSHIFT)) != 0) {

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
            VariablePtr i_result = Variable::CreatePtr(ScalarType::CreatePtr(INT | VARIABLE));
            InstPtr ibin_inst = IBinaryInst::CreatePtr(i_result, op, i_lhs, i_rhs);
            block->insertInst(ibin_inst);
            return i_result;
        } else {
            VariablePtr f_result = Variable::CreatePtr(ScalarType::CreatePtr(FLOAT | VARIABLE));
            InstPtr fbin_inst = FBinaryInst::CreatePtr(f_result, op, f_lhs, f_rhs);
            block->insertInst(fbin_inst);
            return f_result;
        }
    } else if ((op & (OP_LTH | OP_LEQ | OP_GTH | OP_GEQ | OP_EQU | OP_NEQ)) != 0) {
        if (lhs_type != rhs_type) {
            if (lhs_type == BOOL || rhs_type == BOOL) {
                i_lhs = scalarTypeConvert(BOOL, i_lhs, block);
                i_rhs = scalarTypeConvert(BOOL, i_rhs, block);
                lhs_type = rhs_type = BOOL;
            } else if (lhs_type == FLOAT || rhs_type == FLOAT) {
                f_lhs = scalarTypeConvert(FLOAT, f_lhs, block);
                f_rhs = scalarTypeConvert(FLOAT, f_rhs, block);
                lhs_type = rhs_type = FLOAT;
            }
        }
        assert(lhs_type == rhs_type);
        if (lhs_type == FLOAT) {
            VariablePtr f_result = Variable::CreatePtr(ScalarType::CreatePtr(BOOL | VARIABLE));
            InstPtr fcmp_inst = FCmpInst::CreatePtr(f_result, op, f_lhs, f_rhs);
            block->insertInst(fcmp_inst);
            return f_result;
        } else {
            VariablePtr i_result = Variable::CreatePtr(ScalarType::CreatePtr(BOOL | VARIABLE));
            InstPtr icmp_inst = ICmpInst::CreatePtr(i_result, op, i_lhs, i_rhs);
            block->insertInst(icmp_inst);
            return i_result;
        }
    }
    assert(0);
}
