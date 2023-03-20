#pragma once

#include <variant>

#include "baseValue.hh"

using ConstType = std::variant<bool, int32_t, float>;

class Constant : public BaseValue {
private:
    ConstType value;
public:
    Constant(TypeID, ConstType);
    ~Constant() = default;

    template<typename TypeName>
    TypeName getValue() {
        BaseTypePtr base_type = this->getBaseType();
        TypeName ret_value = (base_type->BoolType())    ? TypeName(std::get<bool>(value)) :
                             (base_type->IntType())     ? TypeName(std::get<int32_t>(value)) :
                             (base_type->FloatType())   ? TypeName(std::get<float>(value)) :
                             TypeName(0);
        return ret_value;
    }

    void fixValue(TypeID);

    std::string toString();
};
