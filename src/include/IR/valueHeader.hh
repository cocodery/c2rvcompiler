#pragma once

#include <memory>

#include "value/baseValue.hh"
#include "value/constant.hh"
#include "value/constarray.hh"
#include "value/uninitvar.hh"
#include "value/variable.hh"
#include "value/globalvalue.hh"
#include "instHeader.hh"

#include "instruction/memoryInst.hh"
#include "function/basicblock.hh"

namespace Value {

bool isConstant(BaseValuePtr);

bool bothConstant(BaseValuePtr, BaseValuePtr);

BaseValuePtr unaryOperate(const OpCode, const ConstantPtr);

BaseValuePtr binaryOperate(const OpCode, const ConstantPtr, const ConstantPtr);

BaseValuePtr unaryOperate(const OpCode, BaseValuePtr, BlockPtr);

BaseValuePtr binaryOperate(const OpCode, BaseValuePtr, BaseValuePtr, BlockPtr);

}
