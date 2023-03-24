#include "scalarType.hh"

ScalarType::ScalarType(TypeID _tid) : BaseType(_tid) {
    // assert(this->checkType(BOOL | INT | FLOAT | VOID));
    // will do same check in BaseType::BaseType
}

std::shared_ptr<ScalarType> ScalarType::CreatePtr(TypeID _tid) {
    return std::make_shared<ScalarType>(_tid);
}

std::string ScalarType::toString() {
    assert(this->checkType(BOOL | INT | FLOAT | VOID));

    std::stringstream ss;
    if (this->GlobalType()) {
        ss << "global ";
    }
    if (this->ConstType()) {
        ss << "const ";
    }
    ss << ( this->IntType()     ?   "int" :
            this->FloatType()   ?   "float" :
            this->BoolType()    ?   "bool" :
            this->VoidType()    ?   "void" : // for function define
                                    "error" // un-reachablable, check before
            ); 

    return ss.str();
}

std::string ScalarType::tollvmIR() {
    assert(this->checkType(BOOL | INT | FLOAT | VOID));

    std::stringstream ss;
    // when is CONST, ignroe GLOBAL in llvm-IR
    if (this->ConstType()) {
        ss << "constant ";
    } else if (this->GlobalType()) {
        ss << "global ";
    }
    ss << ( this->IntType()     ?   "i32" :
            this->FloatType()   ?   "float" :
            this->BoolType()    ?   "i1" :
                                    "error"
            );

    return ss.str();
}