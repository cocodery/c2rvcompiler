#pragma once

#include <vector>

#include "baseType.hh"

class ListType;
using ListTypePtr = std::shared_ptr<ListType>;
using ArrDims = std::vector<size_t>;

class ListType : public BaseType {
private:
    ArrDims dims;
    bool omit;
public:
    ListType(TypeID, ArrDims &, bool);

    size_t getArrDims() const;

    ArrDims getDimArray() const;

    static ListTypePtr CreatePtr(TypeID, ArrDims &, bool);

    std::string tollvmIR();
};