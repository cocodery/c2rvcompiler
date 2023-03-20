#pragma once

#include <sstream>
#include <cassert>
#include <memory>

#include "typeHeader.hh"

class BaseValue {
private:
    BaseTypePtr base_type;
public:
    BaseValue(TypeID);
    BaseValue(ListTypePtr);

    ~BaseValue() = default;

    BaseTypePtr getBaseType();

    virtual void fixValue(TypeID) = 0;

    virtual std::string toString() = 0;
};