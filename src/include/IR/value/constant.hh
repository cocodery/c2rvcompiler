#pragma once

#include <iomanip>
#include <variant>

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

    template<typename T> void convert() {
        std::visit([this](auto &&arg) { value = static_cast<T>(arg); }, value);
    }
    void fixValue(TypeID);

    std::shared_ptr<BaseValue> unaryOperate(const std::string &);

    static std::shared_ptr<Constant> CreatePtr(TypeID, ConstType);

    std::string toString();

    std::string tollvmIR();
};

using ConstantPtr = std::shared_ptr<Constant>;
