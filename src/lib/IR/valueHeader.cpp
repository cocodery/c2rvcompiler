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
    }
    assert(0);
}