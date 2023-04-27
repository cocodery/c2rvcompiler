#include "otherInst.hh"

//===-----------------------------------------------------------===//
//                     CallInst Implementation
//===-----------------------------------------------------------===//

CallInst::CallInst(ScalarTypePtr _type, VariablePtr _ret, std::string &_name, RParamList &_list, CfgNodePtr block)
    : ret_type(_type), ret_value(_ret), callee_name(_name), rparam_list(_list), Instruction(block) {
    if (ret_type->VoidType()) {
        assert(_ret == nullptr);
    } else {
        assert(ret_type->getAttrType() == ret_value->getBaseType()->getAttrType());
        assert(ret_value->IsOprand());
    }
    // param-type have been checked at `visitFuncRParams`
}

CallInstPtr CallInst::CreatePtr(ScalarTypePtr _type, VariablePtr _ret, std::string &_name, RParamList &_list,
                                CfgNodePtr block) {
    return std::make_shared<CallInst>(_type, _ret, _name, _list, block);
}

BaseValuePtr CallInst::DoCallFunction(ScalarTypePtr _type, std::string &_name, RParamList &_list, CfgNodePtr block) {
    VariablePtr _ret =
        (_type->VoidType()) ? nullptr : Variable::CreatePtr(_type->IntType() ? type_int_L : type_float_L);
    block->InsertInst(CreatePtr(_type, _ret, _name, _list, block));
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
    ss << "; " << parent->GetBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     BitCastInst Implementation
//===-----------------------------------------------------------===//

BitCastInst::BitCastInst(BaseValuePtr _res, BaseValuePtr _opr, CfgNodePtr block)
    : result(_res), oprand(_opr), Instruction(block) {
    assert(result->getBaseType()->CharType() && result->getBaseType()->IsPointer());
    assert((oprand->getBaseType()->IntType() || oprand->getBaseType()->FloatType()) &&
           result->getBaseType()->IsPointer());
}

BitCastInstPtr BitCastInst::CreatePtr(BaseValuePtr _res, BaseValuePtr _opr, CfgNodePtr block) {
    return std::make_shared<BitCastInst>(_res, _opr, block);
}

BaseValuePtr BitCastInst::DoBitCast(BaseValuePtr _opr, CfgNodePtr block) {
    BaseValuePtr _res = Variable::CreatePtr(type_char_ptr);
    block->InsertInst(CreatePtr(_res, _opr, block));
    return _res;
}

std::string BitCastInst::tollvmIR() {
    std::stringstream ss;

    ss << result->tollvmIR() << " = bitcast ";
    ss << oprand->getBaseType()->tollvmIR() << ' ' << oprand->tollvmIR();
    ss << " to " << result->getBaseType()->tollvmIR();

    ss << "; " << parent->GetBlockIdx();
    return ss.str();
}
