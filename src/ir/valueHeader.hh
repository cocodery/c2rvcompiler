#pragma once

#include <memory>

#include "function/cfgNode.hh"
#include "instHeader.hh"
#include "instruction/memoryInst.hh"
#include "value/baseValue.hh"
#include "value/constant.hh"
#include "value/constarray.hh"
#include "value/globalvalue.hh"
#include "value/uninitvar.hh"
#include "value/variable.hh"

namespace Value {

BaseValuePtr UnaryOperate(const OpCode, const ConstantPtr);

BaseValuePtr BinaryOperate(const OpCode, const ConstantPtr, const ConstantPtr);

BaseValuePtr UnaryOperate(const OpCode, BaseValuePtr, CfgNodePtr);

BaseValuePtr BinaryOperate(const OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

BaseValuePtr ScalarTypeConvert(ATTR_TYPE, BaseValuePtr, CfgNodePtr);

}  // namespace Value
