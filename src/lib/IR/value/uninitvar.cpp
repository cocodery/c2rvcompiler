#include "uninitvar.hh"

UnInitVar::UnInitVar(BaseTypePtr _type) : BaseValue(_type) {
    assert(_type->checkType(INT | FLOAT, GLOBAL, CONST | VARIABLE));
}

UnInitVarPtr UnInitVar::CreatePtr(BaseTypePtr _type) {
    return std::make_shared<UnInitVar>(_type);
}

std::string UnInitVar::tollvmIR() {
    BaseTypePtr base_type = this->getBaseType();
    assert(base_type->checkType(INT | FLOAT));

    std::stringstream ss;
    if (base_type->ArrayType()) {
        ss << "zeroinitializer";
    } else if (base_type->IntType()) {
        ss << "0";
    } else if (base_type->FloatType()) {
        ss << "0x0000000000000000";
    } else {
        assert(0);
    }

    return ss.str();
}
