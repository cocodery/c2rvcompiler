#pragma once

#include <vector>

#include "baseValue.hh"
#include "constant.hh"

using ConstArr = std::vector<ConstantPtr>;

// both global-const-array or local-const-array
// will be put in .data section
class ConstArray : public BaseValue {
private:
    ConstArr const_arr;
    size_t idx; // to distinguish const-array in different scope with same name

    static size_t arr_idx;
public:
    ConstArray(ListTypePtr, ConstArr &);
    ~ConstArray() = default;

    void fixValue(TypeID);

    static std::shared_ptr<ConstArray> CreatePtr(ListTypePtr, ConstArr &);

    std::string toString();

    std::string tollvmIR();
};