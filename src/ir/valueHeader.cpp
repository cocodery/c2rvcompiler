#include "valueHeader.hh"

BaseValuePtr Value::UnaryOperate(const OpCode op, BaseValuePtr oprand, CfgNodePtr block) {
    assert(oprand->IsOprand());
    if (op == OP_ADD) return oprand;
    if (oprand->IsConstant()) {
        return ExprFlod::UnaryOperate(op, std::static_pointer_cast<Constant>(oprand));
    } else {
        ATTR_TYPE _type = oprand->GetBaseType()->GetAttrType();
        ConstantPtr zero = (_type == BOOL) ? zero_bool : ((_type == INT32) ? zero_int32 : zero_float);
        if (op == OP_MINUS) {
            return BinaryOperate(OP_SUB, zero, oprand, block);
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

BaseValuePtr Value::BinaryOperate(const OpCode op, BaseValuePtr lhs, BaseValuePtr rhs, CfgNodePtr block) {
    assert(lhs->IsOprand() && rhs->IsOprand());
    if (lhs->IsConstant() && rhs->IsConstant()) {
        return ExprFlod::BinaryOperate(op, std::static_pointer_cast<Constant>(lhs),
                                       std::static_pointer_cast<Constant>(rhs));
    }
    BaseValuePtr i_lhs = lhs, f_lhs = lhs;
    BaseValuePtr i_rhs = rhs, f_rhs = rhs;

    ATTR_TYPE lhs_type = lhs->GetBaseType()->GetAttrType();
    ATTR_TYPE rhs_type = rhs->GetBaseType()->GetAttrType();

    if (OP_ADD <= op && op <= OP_RSHIFT) {
        // when do arithmetic operation, lhs_type == rhs_type in { INT32, FLOAT }
        if ((lhs_type != rhs_type) || ((lhs_type & rhs_type) == BOOL)) {
            if (lhs_type == FLOAT || rhs_type == FLOAT) {
                f_lhs = ScalarTypeConvert(FLOAT, f_lhs, block);
                f_rhs = ScalarTypeConvert(FLOAT, f_rhs, block);
                lhs_type = rhs_type = FLOAT;
            } else {
                i_lhs = ScalarTypeConvert(INT32, i_lhs, block);
                i_rhs = ScalarTypeConvert(INT32, i_rhs, block);
                lhs_type = rhs_type = INT32;
            }
        }
        assert(lhs_type == rhs_type);
        if (lhs_type == INT32) {
            return IBinaryInst::DoIBinOperate(op, i_lhs, i_rhs, block);
        } else {
            return FBinaryInst::DoFBinOperate(op, f_lhs, f_rhs, block);
        }
    } else if (OP_LTH <= op && op <= OP_NEQ) {
        // when do compare operation, lhs_type == rhs_type in { BOOL, INT32, FLOAT }
        if (lhs_type != rhs_type) {
            if (lhs_type == BOOL || rhs_type == BOOL) {  // if one is BOOL, convert the other to BOOL
                i_lhs = ScalarTypeConvert(BOOL, i_lhs, block);
                i_rhs = ScalarTypeConvert(BOOL, i_rhs, block);
                lhs_type = rhs_type = BOOL;
            } else if (lhs_type == FLOAT || rhs_type == FLOAT) {
                f_lhs = ScalarTypeConvert(FLOAT, f_lhs, block);
                f_rhs = ScalarTypeConvert(FLOAT, f_rhs, block);
                lhs_type = rhs_type = FLOAT;
            }  // else, Both type are INT32
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

BaseValuePtr Value::ScalarTypeConvert(ATTR_TYPE type_convert, BaseValuePtr convertee, CfgNodePtr block) {
    assert(convertee->IsOprand());
    ATTR_TYPE type_convertee = convertee->GetBaseType()->GetAttrType();
    // if type_convert == type_convertee, no need to convert
    if (type_convert == type_convertee) {
        return convertee;
    }
    // if convertee is `CONSTANT`, use `fixType` to convert
    if (convertee->IsConstant()) {
        ConstantPtr constant_convertee = std::static_pointer_cast<Constant>(convertee);
        ScalarTypePtr _stype = (type_convertee == INT32)   ? type_const_int
                               : (type_convertee == FLOAT) ? type_const_float
                                                           : type_const_bool;
        ConstantPtr constant = Constant::CreatePtr(_stype, constant_convertee->GetValue());
        constant->FixValue(type_convert);
        return constant;
    }
    // use instruction to convert
    if (type_convert == FLOAT) {
        // convert i1 or i32 to float
        return SitoFpInst::DoSitoFp(convertee, block);
    } else if (type_convert == INT32) {
        if (type_convertee == FLOAT) {
            // convert float to i32 or i1
            return FptoSiInst::DoFptoSi(type_convert, convertee, block);
        } else if (type_convertee == BOOL) {
            // convert i1 to i32
            return ZextInst::DoZeroExt(convertee, block);
        }
    } else {
        if (type_convertee == INT32) {
            // convert i32 to i1
            return ICmpInst::DoICompare(OP_NEQ, convertee, zero_int32, block);
        } else if (type_convertee == FLOAT) {
            // convert float to i1
            return FCmpInst::DoFCompare(OP_NEQ, convertee, zero_float, block);
        }
    }
    assert(0);
}
