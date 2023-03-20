#pragma once

#include <memory>

#include "value/baseValue.hh"
#include "value/constant.hh"
#include "value/constarray.hh"
#include "value/uninitvar.hh"
#include "value/variable.hh"

using BaseValuePtr  = std::shared_ptr<BaseValue>;
using ConstantPtr   = std::shared_ptr<Constant>;
using ConstArrayPtr = std::shared_ptr<ConstArray>;
using UnInitVarPtr  = std::shared_ptr<UnInitVar>;
using VariablePtr   = std::shared_ptr<Variable>;
