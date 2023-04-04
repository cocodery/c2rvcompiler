#pragma once

#include "baseType.hh"

class ScalarType;
using ScalarTypePtr = std::shared_ptr<ScalarType>;

class ScalarType : public BaseType {
private:
public:
    ScalarType(TypeID);

    static std::shared_ptr<ScalarType> CreatePtr(TypeID);

    std::string tollvmIR();
};