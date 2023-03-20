#include "scalarType.hh"

ScalarType::ScalarType(TypeID _tid) : BaseType(_tid) {

}

std::shared_ptr<ScalarType> ScalarType::CreatePtr(TypeID _tid) {
    return std::make_shared<ScalarType>(_tid);
}

std::string ScalarType::toString() {
    this->checkType(BOOL | INT | FLOAT | VOID); // check here

    std::stringstream ss;
    if (this->ConstType()) {
        ss << "const ";
    }
    
    ss << ( this->IntType()     ?   "int" :
            this->FloatType()   ?   "float" :
            this->BoolType()    ?   "bool" :
            this->VoidType()    ?   "void" :
                                    "error" // un-reachablable, check before
            ); 
    return ss.str();
}

std::string ScalarType::tollvmIR() {
    return "";
}