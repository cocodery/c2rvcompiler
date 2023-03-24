#pragma once

#include <memory>

#include "value/baseValue.hh"
#include "value/constant.hh"
#include "value/constarray.hh"
#include "value/uninitvar.hh"
#include "value/variable.hh"

#include "function/basicblock.hh"

using BaseValuePtr  = std::shared_ptr<BaseValue>;
using ConstantPtr   = std::shared_ptr<Constant>;
using ConstArrayPtr = std::shared_ptr<ConstArray>;
using UnInitVarPtr  = std::shared_ptr<UnInitVar>;
using VariablePtr   = std::shared_ptr<Variable>;

namespace Value {

bool isConstant(BaseValuePtr);

bool bothConstant(BaseValuePtr, BaseValuePtr);

BaseValuePtr unaryOperate(std::string &, BaseValuePtr, BlockPtr);

BaseValuePtr binaryOperate(std::string &, BaseValuePtr, BaseValuePtr, BlockPtr);

}
