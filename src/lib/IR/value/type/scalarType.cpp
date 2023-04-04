#include "scalarType.hh"

ScalarType::ScalarType(TypeID _tid) : BaseType(_tid) {
    // assert(this->checkType(BOOL | INT | FLOAT | VOID));
    // will do same check in BaseType::BaseType
}

ScalarTypePtr ScalarType::CreatePtr(TypeID _tid) {
    return std::make_shared<ScalarType>(_tid);
}

std::string ScalarType::tollvmIR() {
    assert(this->checkType(BOOL | INT | FLOAT | VOID));

    std::stringstream ss;
    ss << ( this->IntType()     ?   "i32" :
            this->FloatType()   ?   "float" :
            this->BoolType()    ?   "i1" :
            this->VoidType()    ?   "void" :
                                    "error"
            );
    if (this->PoniterType()) {
        ss << "*";
    }

    return ss.str();
}