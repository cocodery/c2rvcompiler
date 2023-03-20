#pragma once

#include <memory>

#include "type/baseType.hh"
#include "type/listType.hh"
#include "type/scalarType.hh"

using BaseTypePtr   = std::shared_ptr<BaseType>;
using ListTypePtr   = std::shared_ptr<ListType>;
using ScalarTypePtr = std::shared_ptr<ScalarType>;
