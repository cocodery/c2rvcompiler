#include "valueHeader.hh"

bool Value::isConstant(BaseValuePtr value) {
    BaseTypePtr base_type = value->getBaseType();
    return (base_type->ConstantType() && !base_type->ArrayType());
}

bool Value::bothConstant(BaseValuePtr value1, BaseValuePtr value2) {
    return isConstant(value1) && isConstant(value2);
}

BaseValuePtr Value::unaryOperate(std::string &op, BaseValuePtr value, BlockPtr block) {
    if (op == "+") return value;

    if (isConstant(value)) {
        ConstantPtr constant = std::dynamic_pointer_cast<Constant>(value);
        return constant->unaryOperate(op);
    }
    assert(0);
}

BaseValuePtr Value::binaryOperate(std::string &op, BaseValuePtr lhs, BaseValuePtr rhs, BlockPtr block) {
    if (bothConstant(lhs, rhs)) {
        ConstantPtr constant1 = std::dynamic_pointer_cast<Constant>(lhs);
        ConstantPtr constant2 = std::dynamic_pointer_cast<Constant>(rhs);
        return constant1->binaryOperate(op, constant2);
    } else {
        BaseValuePtr i_lhs = lhs, f_lhs = lhs;
        BaseValuePtr i_rhs = rhs, f_rhs = rhs;

        TypeID lhs_type = lhs->getBaseType()->getMaskedType(BOOL | INT | FLOAT);
        TypeID rhs_type = lhs->getBaseType()->getMaskedType(BOOL | INT | FLOAT);

        if ((lhs_type != rhs_type) || (lhs_type & rhs_type == BOOL)) {
            // TODO: type-convert
        } 
        assert(lhs_type == rhs_type);
        if (lhs_type == INT) {
            VariablePtr i_result = Variable::CreatePtr(ScalarType::CreatePtr(INT | VARIABLE));
            InstPtr ibin_inst = IBinaryInst::CreatePtr(i_result, op[0], i_lhs, i_rhs);
            block->insertInst(ibin_inst);
            return i_result;
        } else {
            VariablePtr f_result = Variable::CreatePtr(ScalarType::CreatePtr(FLOAT | VARIABLE));
            InstPtr fbin_inst = IBinaryInst::CreatePtr(f_result, op[0], f_lhs, f_rhs);
            block->insertInst(fbin_inst);
            return f_result;
        }
    }
}