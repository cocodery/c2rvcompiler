#pragma once

#include <vector>

#include "baseValue.hh"
#include "constant.hh"

class ConstArray;
using ConstArrayPtr = std::shared_ptr<ConstArray>;
using ConstArr = std::vector<ConstantPtr>;

// both global-const-array or local-const-array
// will be put in .data section
class ConstArray : public BaseValue {
private:
    ConstArr const_arr;
public:
    ConstArray(ListTypePtr, ConstArr &);
    ~ConstArray() = default;

    void fixValue(TypeID);

    static ConstArrayPtr CreatePtr(ListTypePtr, ConstArr &);

    std::string toString();

    std::string tollvmIR();
};