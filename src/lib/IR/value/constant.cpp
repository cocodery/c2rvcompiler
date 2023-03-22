#include "constant.hh"

Constant::Constant(TypeID _tid, ConstType _value) : BaseValue(_tid), value(_value) {
    assert((_tid & (CONST | BOOL | INT | FLOAT)) & CONST);
}

void Constant::fixValue(TypeID _tid) {
    BaseTypePtr base_type = this->getBaseType();

    if (_tid & BOOL) {
        if (base_type->IntType()) {
            value = bool(this->getValue<int32_t>());
        } else if (base_type->FloatType()) {
            value = bool(this->getValue<float>());
        }
    } else if (_tid & INT) {
        if (base_type->BoolType()) {
            value = int32_t(this->getValue<bool>());
        } else if (base_type->FloatType()) {
            value = int32_t(this->getValue<float>());
        }
    } else if (_tid & FLOAT) {
        if (base_type->BoolType()) {
            value = float(this->getValue<bool>());
        } else if (base_type->IntType()) {
            value = float(this->getValue<int32_t>());
        }
    }
    base_type->resetType(_tid);
} 

std::shared_ptr<Constant> Constant::CreatePtr(TypeID _tid, ConstType _value) {
    return std::make_shared<Constant>(_tid, _value);
}

std::string Constant::toString() {
    BaseTypePtr base_type = this->getBaseType();
    base_type->checkType(BOOL | INT | FLOAT | CONST);
    std::stringstream ss;
    ss << base_type->toString();
    ss << " -> ";
    ss <<   (   base_type->BoolType()   ?   this->getValue<bool>() :
                base_type->IntType()    ?   this->getValue<int32_t>() :
                base_type->FloatType()  ?   this->getValue<float>() :
                0
            );
    return ss.str();
}

std::string Constant::tollvmIR() {
    BaseTypePtr base_type = this->getBaseType();

    std::stringstream ss;

    ss << base_type->tollvmIR() << ' ';

    if (base_type->BoolType()) {
        ss << this->getValue<bool>();
    } else if (base_type->IntType()) {
        ss << this->getValue<int32_t>();
    } else if (base_type->FloatType()) {
        double float_value = double(this->getValue<float>());
        uint64_t uint64_value = reinterpret_cast<uint64_t &>(float_value);
        ss << "0x" << std::hex << std::setw(16) << std::setfill('0') << uint64_value;
    } else {
        assert(0);
    }

    return ss.str();
}
