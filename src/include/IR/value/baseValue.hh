#pragma once

#include <string>
#include <sstream>
#include <cassert>
#include <memory>

#include "typeHeader.hh"

class BaseValue;
using BaseValuePtr = std::shared_ptr<BaseValue>;

class BaseValue {
protected:
    BaseTypePtr base_type;
public:
    BaseValue(BaseTypePtr);
    ~BaseValue() = default;

    BaseTypePtr getBaseType();

    bool IsOprand();

    virtual bool IsConstant() = 0;

    virtual void fixValue(ATTR_TYPE) = 0;

    virtual std::string tollvmIR() = 0;
};

std::ostream &operator<<(std::ostream &, BaseValuePtr);