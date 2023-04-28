#pragma once

#include <cassert>
#include <memory>
#include <sstream>
#include <string>

#include "type/baseType.hh"
#include "type/listType.hh"
#include "type/scalarType.hh"
#include "use.hh"

class BaseValue;
using BaseValuePtr = std::shared_ptr<BaseValue>;

class Instruction;
using InstPtr = std::shared_ptr<Instruction>;

class BaseValue : public Use {
   protected:
    BaseTypePtr base_type;

   public:
    BaseValue(BaseTypePtr);
    ~BaseValue() = default;

    BaseTypePtr getBaseType();

    bool IsOprand();

    virtual const InstPtr GetParent() const;

    virtual bool IsConstant();

    virtual void fixValue(ATTR_TYPE);

    InstList GetUserList() const;
    void InsertUser(InstPtr);
    void RemoveUser(InstPtr);

    virtual std::string tollvmIR() = 0;
};