#include "controlFlowInst.hh"

//===-----------------------------------------------------------===//
//                     ReturnInst Implementation
//===-----------------------------------------------------------===//

ReturnInst::ReturnInst(ScalarTypePtr _type, BaseValuePtr _value)
    : ret_type(_type), ret_value(_value) {
}

RetInstPtr ReturnInst::CreatePtr(ScalarTypePtr _type, BaseValuePtr _value) {
    return std::make_shared<ReturnInst>(_type, _value);
}

std::string ReturnInst::tollvmIR() {
    std::stringstream ss;
    if (ret_type->checkType(VOID)) {
        assert(ret_value == nullptr);
        ss << "ret void";
    } else {
        assert(ret_type->getMaskedType(INT | FLOAT) == ret_value->getBaseType()->getMaskedType(INT | FLOAT));
        ss << "ret " << ret_type->tollvmIR() << ' ';
        ss << ret_value->tollvmIR();
    }
    return ss.str();
}