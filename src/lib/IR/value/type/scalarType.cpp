#include "scalarType.hh"

ScalarType::ScalarType(BaseType _type) 
    : BaseType(_type) {
    // no need to check for ScalarType
}

ScalarTypePtr ScalarType::CreatePtr(BaseType _type) {
    return std::make_shared<ScalarType>(_type);
}

std::string ScalarType::tollvmIR() {
    std::stringstream ss;

    switch (attr_type) {
        case VOID : ss << "void" ; break;
        case BOOL : ss << "i1"   ; break;
        case CHAR : ss << "i8"   ; break;
        case INT  : ss << "i32"  ; break;
        case FLOAT: ss << "float"; break;
        default: assert(0); break;
    }
    if (attr_pointer == POINTER) {
        ss << "*";
    }

    return ss.str();
}