#pragma once

#include <iomanip>
#include <variant>
#include <type_traits>

#include "baseValue.hh"

using ConstType = std::variant<bool, int32_t, float>;

class Constant : public BaseValue {
private:
    ConstType value;

    /* to allow access from AstVisitor::getArrDims() */
    friend class AstVisitor;
public:
    Constant(TypeID, ConstType);
    ~Constant() = default;

    ConstType &getValue();

    void fixValue(TypeID);

    std::shared_ptr<BaseValue> unaryOperate(const std::string &);

    std::shared_ptr<BaseValue> binaryOperate(const std::string &, const std::shared_ptr<Constant>);

    static std::shared_ptr<Constant> CreatePtr(TypeID, ConstType);

    std::string toString();

    std::string tollvmIR();
};

using ConstantPtr = std::shared_ptr<Constant>;
