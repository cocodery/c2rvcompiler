#pragma once

#include "baseType.hh"

class ScalarType : public BaseType {
private:
public:
    ScalarType(TypeID);

    static std::shared_ptr<ScalarType> CreatePtr(TypeID);

    std::string toString();
    std::string tollvmIR();
};