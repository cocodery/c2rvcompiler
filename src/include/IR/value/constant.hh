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

    bool isConstant() { return true; } ;

    void fixValue(ATTR_TYPE);

    static ConstantPtr CreatePtr(ScalarTypePtr, ConstType);

    std::string tollvmIR();
};

static const ConstantPtr zero_int32 = Constant::CreatePtr(type_const_int, static_cast<int32_t>(0));
static const ConstantPtr zero_float = Constant::CreatePtr(type_const_float, static_cast<float>(0));
static const ConstantPtr zero_bool  = Constant::CreatePtr(type_const_bool, static_cast<bool>(0));
