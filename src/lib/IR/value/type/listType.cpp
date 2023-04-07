#include "listType.hh"

ListType::ListType(TypeID _tid, ArrDims &_dims, bool _omit) 
    : BaseType(_tid), dims(_dims), omit(_omit) {
    assert(this->checkType(INT | FLOAT, ARRAY));
}

size_t ListType::getArrDims() const {
    size_t size = 1;
    for (auto &&dim : dims) {
        size *= dim;
    }
    return size;
}

ArrDims ListType::getDimArray() const {
    return this->dims;
}

ListTypePtr ListType::CreatePtr(TypeID _tid, ArrDims &_dims, bool _omit) {
    return std::make_shared<ListType>(_tid, _dims, _omit);
}

std::string ListType::tollvmIR() {
    assert(this->checkType(INT | FLOAT, ARRAY));

    std::stringstream ss;
    ss << '[' << this->getArrDims() << " x ";
    ss << ( this->IntType()     ?   "i32" :
            this->FloatType()   ?   "float" :
                                    "error"
            );
    ss << ']';
    if (this->PoniterType()) {
        ss << "*";
    }

    return ss.str();
}