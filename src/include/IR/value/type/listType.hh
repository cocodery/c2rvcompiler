#pragma once

#include <vector>

#include "baseType.hh"

class ListType;
using ListTypePtr = std::shared_ptr<ListType>;
using ArrDims = std::vector<size_t>;

class ListType : public BaseType {
private:
    ArrDims dims;
public:
    ListType(ATTR_TYPE, ATTR_MUTABLE, ATTR_POINTER, ATTR_SCALAR, ATTR_POSITION, ArrDims &);

    size_t getArrSize() const;

    const ArrDims getDimSize() const;

    const ArrDims getArrDims() const;

    static ListTypePtr CreatePtr(ATTR_TYPE, ATTR_MUTABLE, ATTR_POINTER, ATTR_SCALAR, ATTR_POSITION, ArrDims &);

    std::string tollvmIR();
};