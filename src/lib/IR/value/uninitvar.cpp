#include "uninitvar.hh"

UnInitVar::UnInitVar(BaseTypePtr _type) 
    : BaseValue(_type) {
    // INT || FLOAT
    assert(base_type->intType() || base_type->floatType());
    // MUTABLE, NOTPTR, GLOBAL
    assert(base_type->IsMutable() && base_type->IsNotPtr() && base_type->IsGlobal());
}

UnInitVarPtr UnInitVar::CreatePtr(BaseTypePtr _type) {
    return std::make_shared<UnInitVar>(_type);
}

std::string UnInitVar::tollvmIR() {
    std::stringstream ss;

    if (!base_type->IsScalar()) {
        ss << "zeroinitializer";
    } else if (base_type->intType()) {
        ss << "0";
    } else if (base_type->floatType()) {
        ss << "0x0000000000000000";
    } else {
        assert(0);
    }

    return ss.str();
}
