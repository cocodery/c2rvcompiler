#pragma once

#include "baseType.hh"

class ScalarType : public BaseType {
private:
public:
    ScalarType(TypeID);

    std::string toString();
    std::string tollvmIR();
};