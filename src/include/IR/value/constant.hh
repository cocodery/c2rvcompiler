#pragma once

#include <iomanip>
#include <variant>
#include <type_traits>

#include "baseValue.hh"

class Constant;
using ConstantPtr = std::shared_ptr<Constant>;
using ConstType = std::variant<bool, int32_t, float>;

class Constant : public BaseValue {
private:
    ConstType value;
public:
    Constant(ScalarTypePtr, ConstType);
    ~Constant() = default;

    const ConstType &getValue() const;

    void fixValue(ATTR_TYPE);

    static ConstantPtr CreatePtr(ScalarTypePtr, ConstType);

    std::string tollvmIR();
};

static ConstantPtr zero_int32 = Constant::CreatePtr(ScalarType::CreatePtr(INT  , IMMUTABLE, NOTPTR, SCALAR, NONE4), static_cast<int32_t>(0));
static ConstantPtr zero_float = Constant::CreatePtr(ScalarType::CreatePtr(FLOAT, IMMUTABLE, NOTPTR, SCALAR, NONE4), static_cast<float>(0));
static ConstantPtr zero_bool  = Constant::CreatePtr(ScalarType::CreatePtr(BOOL , IMMUTABLE, NOTPTR, SCALAR, NONE4), static_cast<bool>(0));
