#include "constarray.hh"

ConstArray::ConstArray(ListTypePtr list_type, ConstArr &_arr)
    : const_arr(_arr), BaseValue(list_type) {

}

void ConstArray::fixValue(TypeID _tid) {
    assert(this->getBaseType()->getType() == (_tid | ARRAY));
    if (_tid & GLOBAL) {
        _tid &= (~GLOBAL);
    }
    if (_tid & CONST) {
        _tid &= (~CONST);
    }
    for (auto &&value : const_arr) {
        value->fixValue(_tid);
    }
}

std::shared_ptr<ConstArray> ConstArray::CreatePtr(ListTypePtr list_type, ConstArr &_arr) {
    return std::make_shared<ConstArray>(list_type, _arr);
}

std::string ConstArray::toString() {
    BaseTypePtr base_type = this->getBaseType();
    base_type->checkType(INT | FLOAT, CONST, ARRAY);
    std::stringstream ss;
    ss << base_type->toString();
    ss << " -> { ";
    for (auto &&value : const_arr) {
        ss << value->toString() << ' ';
    }
    ss << "}";
    return ss.str();
}

std::string ConstArray::tollvmIR() {
    BaseTypePtr base_type = this->getBaseType();

    std::stringstream ss;

    ss << base_type->tollvmIR() << ' ';
    
    ss << '[';
    size_t size = const_arr.size();
    ss << const_arr[0]->tollvmIR();
    for (size_t idx = 1; idx < size; ++idx) {
        ss << ", " << const_arr[idx]->tollvmIR();
    }
    ss << ']';

    return ss.str();
}