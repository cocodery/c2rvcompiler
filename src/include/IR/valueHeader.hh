#pragma once

#include <memory>

#include "value/baseValue.hh"
#include "value/constant.hh"
#include "value/constarray.hh"
#include "value/uninitvar.hh"
#include "value/variable.hh"

#include "function/basicblock.hh"

namespace Value {

bool isConstant(BaseValuePtr);

bool bothConstant(BaseValuePtr, BaseValuePtr);

BaseValuePtr unaryOperate(std::string &, BaseValuePtr, BlockPtr);

BaseValuePtr binaryOperate(std::string &, BaseValuePtr, BaseValuePtr, BlockPtr);

}
