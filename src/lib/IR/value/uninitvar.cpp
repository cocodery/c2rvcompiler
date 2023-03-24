#include "uninitvar.hh"

UnInitVar::UnInitVar(TypeID _tid) : BaseValue(_tid) {

}

UnInitVar::UnInitVar(ListTypePtr list_type) : BaseValue(list_type) {

}

std::shared_ptr<UnInitVar> UnInitVar::CreatePtr(TypeID _tid) {
    return std::make_shared<UnInitVar>(_tid);
}

std::shared_ptr<UnInitVar> UnInitVar::CreatePtr(ListTypePtr list_type) {
    return std::make_shared<UnInitVar>(list_type);
}

std::string UnInitVar::toString() {
    BaseTypePtr base_tpye = this->getBaseType();
    
    std::stringstream ss;
    ss << base_tpye->toString();
    if (!base_tpye->ParamType()) {
        ss << " -> ";
        if (base_tpye->ArrayType()) {
            ss << "zeroinitializer";
        } else {
            if (base_tpye->IntType()) {
                ss << "i32 0";
            } else if (base_tpye->FloatType()) {
                ss << "float 0";
            }
        }
    }
    return ss.str();
}

std::string UnInitVar::tollvmIR() {
    BaseTypePtr base_type = this->getBaseType();

    std::stringstream ss;

    ss << base_type->tollvmIR() << ' ';

    if (base_type->ArrayType()) {
        ss << "zeroinitializer";
    } else if (base_type->IntType()) {
        ss << "i32 0";
    } else if (base_type->FloatType()) {
        ss << "float 0x0000000000000000";
    } else {
        assert(0);
    }
    
    return ss.str();
}