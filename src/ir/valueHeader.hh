#pragma once

#include <memory>

#include "exprFlod.hh"
#include "function/cfgNode.hh"
#include "instHeader.hh"
#include "value/baseValue.hh"
#include "value/constant.hh"
#include "value/constarray.hh"
#include "value/globalvalue.hh"
#include "value/uninitvar.hh"
#include "value/variable.hh"

namespace Value {

BaseValuePtr UnaryOperate(const OpCode, BaseValuePtr, CfgNodePtr);

BaseValuePtr BinaryOperate(const OpCode, BaseValuePtr, BaseValuePtr, CfgNodePtr);

BaseValuePtr ScalarTypeConvert(ATTR_TYPE, BaseValuePtr, CfgNodePtr);

bool ValueCompare(const BaseValuePtr, const BaseValuePtr);

BaseValuePtr FixValue(const ATTR_TYPE, BaseValuePtr);

}  // namespace Value
