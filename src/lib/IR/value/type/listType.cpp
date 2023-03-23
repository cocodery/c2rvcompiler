#include "listType.hh"

ListType::ListType(TypeID _tid, ArrDims &_dims, bool _omit) 
    : BaseType(_tid), dims(_dims), omit(_omit) {
    this->checkType(INT | FLOAT, ARRAY);
}

size_t ListType::getArrDims() const {
    size_t size = 1;
    for (auto &&dim : dims) {
        size *= dim;
    }
    return size;
}

std::shared_ptr<ListType> ListType::CreatePtr(TypeID _tid, ArrDims &_dims, bool _omit) {
    return std::make_shared<ListType>(_tid, _dims, _omit);
}

std::string ListType::toString() {
    this->checkType(INT | FLOAT, ARRAY);

    std::stringstream ss;
    if (this->GlobalType()) {
        ss << "global ";
    }
    if (this->ConstType()) {
        ss << "const ";
    }
    
    ss << ( this->IntType()     ?   "int" :
            this->FloatType()   ?   "float" :
                                    "error" // un-reachablable, check before
            ); 
    if (this->omit) ss << "[]";
    for (auto &&dim : dims) {
        ss << '[' << dim << ']';
    }

    return ss.str();
}

std::string ListType::tollvmIR() {
    // check    
    std::stringstream ss;

    if (this->ConstType()) {
        ss << "constant ";
    } else if (this->GlobalType()) {
        ss << "global ";
    }
    ss << '[' << this->getArrDims() << " x ";
    ss << ( this->IntType()     ?   "i32" :
            this->FloatType()   ?   "float" :
                                    "error"
            );
    ss << ']';

    return ss.str();
}