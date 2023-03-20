#pragma once

#include <vector>

#include "baseType.hh"

using ArrDims = std::vector<size_t>;

class ListType : public BaseType {
private:
    ArrDims dims;
    bool omit;
public:
    ListType(TypeID, ArrDims &, bool);

    size_t getArrDims() const;

    static std::shared_ptr<ListType> CreatePtr(TypeID, ArrDims &, bool);

    std::string toString();
    std::string tollvmIR();
};