#pragma once

#include <vector>

#include "baseValue.hh"
#include "constant.hh"

using ConstArr = std::vector<ConstantPtr>;

class ConstArray : public BaseValue {
private:
    ConstArr const_arr;
public:
    ConstArray(ListTypePtr, ConstArr &);
    ~ConstArray() = default;

    void fixValue(TypeID);

    static std::shared_ptr<ConstArray> CreatePtr(ListTypePtr, ConstArr &);

    std::string toString();
};