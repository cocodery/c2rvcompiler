#include "otherInst.hh"

//===-----------------------------------------------------------===//
//                     CallInst Implementation
//===-----------------------------------------------------------===//

CallInst::CallInst(ScalarTypePtr _type, VariablePtr _ret, std::string &_name, RParamList &_list)
    : ret_type(_type), ret_value(_ret), callee_name(_name), rparam_list(_list) {
    if (ret_type->voidType()) {
        assert(_ret == nullptr);
    } else {
        assert(ret_type->getAttrType() == ret_value->getBaseType()->getAttrType());
        assert(ret_value->isBinaryOprand());
    }
    // param-type have been checked at `visitFuncRParams`
}

CallInstPtr CallInst::CreatePtr(ScalarTypePtr _type, VariablePtr _ret, std::string &_name, RParamList &_list) {
    return std::make_shared<CallInst>(_type, _ret, _name, _list);
}

BaseValuePtr CallInst::DoCallFunction(ScalarTypePtr _type, std::string &_name, RParamList &_list, BlockPtr block) {
    VariablePtr _ret = (_type->voidType()) ? nullptr : Variable::CreatePtr(ScalarType::CreatePtr(_type->getAttrType(), MUTABLE, NOTPTR, SCALAR, LOCAL));
    block->insertInst(CreatePtr(_type, _ret, _name, _list));
    return _ret;
}

std::string CallInst::tollvmIR() {
    std::stringstream ss;
    if (ret_value != nullptr) {
        ss << ret_value->tollvmIR() << " = ";
    }
    ss << "call " << ret_type->tollvmIR() << " @" << callee_name;
    ss << "(";
    size_t rparam_size = rparam_list.size();
    if (rparam_size > 0) {
        size_t idx = 0;
        ss << rparam_list[idx]->getBaseType()->tollvmIR() << ' ' << rparam_list[idx]->tollvmIR();
        for (idx = 1; idx < rparam_size; ++idx) {
            ss << ", " << rparam_list[idx]->getBaseType()->tollvmIR() << ' ' << rparam_list[idx]->tollvmIR();
        }
    }
    ss << ")";
    return ss.str();
}