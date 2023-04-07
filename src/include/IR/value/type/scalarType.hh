#pragma once

#include "baseType.hh"

class ScalarType;
using ScalarTypePtr = std::shared_ptr<ScalarType>;

class ScalarType : public BaseType {
public:
    ScalarType(BaseType);

    static std::shared_ptr<ScalarType> CreatePtr(BaseType);

    std::string tollvmIR();
};

// only use for Function-Return-Type
static const ScalarTypePtr ret_void  = ScalarType::CreatePtr(BaseType(VOID , NONE1, NONE2, NONE3, NONE4));
static const ScalarTypePtr ret_int   = ScalarType::CreatePtr(BaseType(INT  , NONE1, NONE2, NONE3, NONE4));
static const ScalarTypePtr ret_float = ScalarType::CreatePtr(BaseType(FLOAT, NONE1, NONE2, NONE3, NONE4));
