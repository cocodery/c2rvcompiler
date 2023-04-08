#pragma once

#include "baseType.hh"

class ScalarType;
using ScalarTypePtr = std::shared_ptr<ScalarType>;

class ScalarType : public BaseType {
public:
    ScalarType(ATTR_TYPE, ATTR_MUTABLE, ATTR_POINTER, ATTR_SCALAR, ATTR_POSITION);

    static std::shared_ptr<ScalarType> CreatePtr(ATTR_TYPE, ATTR_MUTABLE, ATTR_POINTER, ATTR_SCALAR, ATTR_POSITION);

    std::string tollvmIR();
};

// only use for Function-Return-Type
static const ScalarTypePtr ret_void      = ScalarType::CreatePtr(VOID , NONE1  , NONE2  , NONE3 , NONE4);
static const ScalarTypePtr ret_int       = ScalarType::CreatePtr(INT  , MUTABLE, NOTPTR , SCALAR, LOCAL);
static const ScalarTypePtr ret_int_ptr   = ScalarType::CreatePtr(INT  , MUTABLE, POINTER, SCALAR, LOCAL);
static const ScalarTypePtr ret_float     = ScalarType::CreatePtr(FLOAT, MUTABLE, NOTPTR , SCALAR, LOCAL);
static const ScalarTypePtr ret_float_ptr = ScalarType::CreatePtr(FLOAT, MUTABLE, POINTER, SCALAR, LOCAL);
