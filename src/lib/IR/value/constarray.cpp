#include "constarray.hh"

size_t ConstArray::arr_idx = 1;

ConstArray::ConstArray(ListTypePtr list_type, ConstArr &_arr)
    : const_arr(_arr), idx(arr_idx++), BaseValue(list_type) {

}

void ConstArray::fixValue(TypeID _tid) {
    BaseTypePtr base_type = this->getBaseType();
    assert(base_type->checkType(INT | FLOAT, ARRAY, CONST | VARIABLE));

    // un-set GLOBAL and CONST bit
    // which can emit extra type qualifier in array-value 
    _tid &= ~(GLOBAL | CONST);
    for (auto &&value : const_arr) {
        value->fixValue(_tid);
    }
}

ConstArrayPtr ConstArray::CreatePtr(ListTypePtr list_type, ConstArr &_arr) {
    return std::make_shared<ConstArray>(list_type, _arr);
}

std::string ConstArray::toString() {
    BaseTypePtr base_type = this->getBaseType();
    assert(base_type->checkType(INT | FLOAT, ARRAY, CONST | VARIABLE));

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
    assert(base_type->checkType(INT | FLOAT, ARRAY, CONST | VARIABLE));

    std::stringstream ss;
    ss << '[';
    size_t size = const_arr.size();
    ss << const_arr[0]->getBaseType()->tollvmIR() << ' ' << const_arr[0]->tollvmIR();
    for (size_t idx = 1; idx < size; ++idx) {
        ss << ", " << const_arr[idx]->getBaseType()->tollvmIR() << ' ' << const_arr[idx]->tollvmIR();
    }
    ss << ']';

    return ss.str();
}