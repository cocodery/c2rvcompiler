#pragma once

#include <memory>
#include <variant>

#include "instruction/opCode.hh"
#include "value/baseValue.hh"
#include "value/constant.hh"
#include "value/type/baseType.hh"
#include "value/type/scalarType.hh"

namespace ExprFlod {

BaseValuePtr UnaryOperate(const OpCode, const ConstantPtr);

BaseValuePtr BinaryOperate(const OpCode, const ConstantPtr, const ConstantPtr);
}  // namespace ExprFlod