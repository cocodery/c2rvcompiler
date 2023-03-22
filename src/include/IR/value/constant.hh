#pragma once

#include <iomanip>
#include <variant>

#include "baseValue.hh"

using ConstType = std::variant<bool, int32_t, float>;

class Constant : public BaseValue {
private:
    ConstType value;
public:
    Constant(TypeID, ConstType);
    ~Constant() = default;

    template<typename T> void convert() {
        std::visit([&value](auto &&arg) { value = static_cast<T>(arg); }, value);
    }
    void fixValue(TypeID);


    /* to be removed later */
    template<typename TypeName>
    TypeName getValue() {
        BaseTypePtr base_type = this->getBaseType();
        base_type->checkType(BOOL, INT, FLOAT);
        TypeName ret_value = (base_type->BoolType())    ? TypeName(std::get<bool>(value)) :
                             (base_type->IntType())     ? TypeName(std::get<int32_t>(value)) :
                             (base_type->FloatType())   ? TypeName(std::get<float>(value)) :
                             TypeName(0);
        return ret_value;
    }

    std::shared_ptr<BaseValue> unaryOperate(const std::string &);

    static std::shared_ptr<Constant> CreatePtr(TypeID, ConstType);

    std::string toString();

    std::string tollvmIR();
};

using ConstantPtr = std::shared_ptr<Constant>;
