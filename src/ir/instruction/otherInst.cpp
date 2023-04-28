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
        (_type->VoidType()) ? nullptr : Variable::CreatePtr(_type->IntType() ? type_int_L : type_float_L, nullptr);
    auto &&inst = CreatePtr(_type, _ret, _name, _list, block);
    if (_ret != nullptr) _ret->SetParent(inst);
    std::for_each(_list.begin(), _list.end(), [&inst](const auto &param) { param->InsertUser(inst); });
    block->InsertInstBack(inst);
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
    ss << "; Inst_" << GetInstIdx() << " from Block_" << parent->GetBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     BitCastInst Implementation
//===-----------------------------------------------------------===//

BitCastInst::BitCastInst(VariablePtr _res, BaseValuePtr _opr, CfgNodePtr block) : UnaryInstruction(_res, _opr, block) {
    assert(result->getBaseType()->CharType() && result->getBaseType()->IsPointer());
    assert((oprand->getBaseType()->IntType() || oprand->getBaseType()->FloatType()) &&
           result->getBaseType()->IsPointer());
}

BitCastInstPtr BitCastInst::CreatePtr(VariablePtr _res, BaseValuePtr _opr, CfgNodePtr block) {
    return std::make_shared<BitCastInst>(_res, _opr, block);
}

VariablePtr BitCastInst::DoBitCast(BaseValuePtr _opr, CfgNodePtr block) {
    VariablePtr _res = Variable::CreatePtr(type_char_ptr, nullptr);
    auto &&inst = CreatePtr(_res, _opr, block);
    _res->SetParent(inst);
    _opr->InsertUser(inst);
    block->InsertInstBack(inst);
    return _res;
}

std::string BitCastInst::tollvmIR() {
    std::stringstream ss;

    ss << result->tollvmIR() << " = bitcast ";
    ss << oprand->getBaseType()->tollvmIR() << ' ' << oprand->tollvmIR();
    ss << " to " << result->getBaseType()->tollvmIR();

    ss << "; Inst_" << GetInstIdx() << " from Block_" << parent->GetBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     PhiInst Implementation
//===-----------------------------------------------------------===//

PhiInst::PhiInst(VariablePtr _res, CfgNodePtr block) : result(_res), Instruction(block) {}

PhiInstPtr PhiInst::CreatePtr(BaseTypePtr _type, CfgNodePtr block) {
    assert(_type->IsScalar() && _type->IsNotPtr());
    VariablePtr _res = Variable::CreatePtr(_type, nullptr);
    auto &&inst = std::make_shared<PhiInst>(_res, block);
    _res->SetParent(inst);
    block->InsertInstFront(inst);
    return inst;
}

void PhiInst::InsertPhiData(PhiInstPtr inst, BaseValuePtr _value, CfgNodePtr block) {
    inst->datalist.push_back({_value, block});
    _value->InsertUser(inst);
}

bool PhiInst::IsPhiInst() const { return true; }

std::string PhiInst::tollvmIR() {
    std::stringstream ss;

    ss << result->tollvmIR() << " = phi " << result->getBaseType()->tollvmIR() << ' ';
    auto &&iter = datalist.begin();
    ss << '[' << (*iter).first->tollvmIR() << ", %Block_" << (*iter).second->GetBlockIdx() << ']';
    for (iter++; iter != datalist.end(); iter++) {
        ss << ", " << (*iter).first->tollvmIR() << ", %Block_" << (*iter).second->GetBlockIdx() << ']';
    }

    ss << "; Inst_" << GetInstIdx() << " from Block_" << parent->GetBlockIdx();
    return ss.str();
}
