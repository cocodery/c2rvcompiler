#pragma once

#include <memory>

#include "function/basicblock.hh"
#include "instHeader.hh"
#include "instruction/memoryInst.hh"
#include "value/baseValue.hh"
#include "value/constant.hh"
#include "value/constarray.hh"
#include "value/globalvalue.hh"
#include "value/uninitvar.hh"
#include "value/variable.hh"

namespace Value {

BaseValuePtr unaryOperate(const OpCode, const ConstantPtr);

BaseValuePtr binaryOperate(const OpCode, const ConstantPtr, const ConstantPtr);

BaseValuePtr unaryOperate(const OpCode, BaseValuePtr, BlockPtr);

BaseValuePtr binaryOperate(const OpCode, BaseValuePtr, BaseValuePtr, BlockPtr);

BaseValuePtr scalarTypeConvert(ATTR_TYPE, BaseValuePtr, BlockPtr);

}  // namespace Value
