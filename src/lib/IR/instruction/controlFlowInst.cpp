#include "controlFlowInst.hh"

//===-----------------------------------------------------------===//
//                     ReturnInst Implementation
//===-----------------------------------------------------------===//

ReturnInst::ReturnInst(ScalarTypePtr _type, BaseValuePtr _value)
    : ret_type(_type), ret_value(_value) {
    if (_type->checkType(VOID)) {
        assert(_value == nullptr);
    } else {
        BaseTypePtr ret_value_type = _value->getBaseType();
        assert(ret_type->getMaskedType(INT | FLOAT) == ret_value_type->getMaskedType(INT | FLOAT));
    }
}

RetInstPtr ReturnInst::CreatePtr(ScalarTypePtr _type, BaseValuePtr _value) {
    return std::make_shared<ReturnInst>(_type, _value);
}

std::string ReturnInst::tollvmIR() {
    std::stringstream ss;
    if (ret_type->VoidType()) {
        ss << "ret void";
    } else {
        ss << "ret " << ret_type->tollvmIR() << ' ' << ret_value->tollvmIR();
    }
    return ss.str();
}