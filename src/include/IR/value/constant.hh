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

    /* to allow access from AstVisitor::getArrDims() */
    friend class AstVisitor;
public:
    Constant(ScalarTypePtr, ConstType);
    ~Constant() = default;

    ConstType &getValue();

    void fixValue(TypeID);

    BaseValuePtr unaryOperate(const std::string &);

    BaseValuePtr binaryOperate(const std::string &, const ConstantPtr);

    static ConstantPtr CreatePtr(ScalarTypePtr, ConstType);

    std::string toString();

    std::string tollvmIR();
};
