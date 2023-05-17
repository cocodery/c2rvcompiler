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
using BaseValueList = std::list<BaseValuePtr>;
using ParamList = std::vector<BaseValuePtr>;

class Instruction;
using InstPtr = std::shared_ptr<Instruction>;

class BaseValue : public Use {
   protected:
    BaseTypePtr base_type;
    size_t glb_uniq_idx_;

   public:
    BaseValue(BaseTypePtr);
    ~BaseValue() = default;

    BaseTypePtr GetBaseType();

    bool IsOprand();

    virtual const InstPtr GetParent() const;

    virtual bool IsConstant() const;
    virtual bool IsConstArray() const;
    virtual bool IsGlobalValue() const;
    virtual bool IsUnInitVar() const;
    virtual bool IsVariable() const;

    virtual void FixValue(ATTR_TYPE);

    InstList GetUserList() const;
    void InsertUser(InstPtr);
    void RemoveUser(InstPtr);

    size_t guidx() const;

    virtual std::string tollvmIR() = 0;
};