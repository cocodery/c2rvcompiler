#pragma once

#include <memory>
#include <variant>

#include "baseType.hh"
#include "baseValue.hh"
#include "constant.hh"
#include "opCode.hh"
#include "scalarType.hh"

namespace ExprFlod {

BaseValuePtr UnaryOperate(const OpCode, const ConstantPtr);

BaseValuePtr BinaryOperate(const OpCode, const ConstantPtr, const ConstantPtr);
}  // namespace ExprFlod