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

//===-----------------------------------------------------------===//
//                     CallInst Implementation
//===-----------------------------------------------------------===//

CallInst::CallInst(ScalarTypePtr _type, BaseValuePtr _ret, std::string &_name, RParamList &_list)
    : ret_type(_type), ret_value(_ret), callee_name(_name), rparam_list(_list) {
    if (_type->VoidType()) {
        assert(_ret == nullptr);
    }
    // param-type have been checked at `visitFuncRParams`
}

CallInstPtr CallInst::CreatePtr(ScalarTypePtr _type, BaseValuePtr _ret, std::string &_name, RParamList &_list) {
    return std::make_shared<CallInst>(_type, _ret, _name, _list);
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

//===-----------------------------------------------------------===//
//                     JumpInst Implementation
//===-----------------------------------------------------------===//

JumpInst::JumpInst(BlockPtr _dest) : dest(_dest) { }

JumpInstPtr JumpInst::CreatePtr(BlockPtr _dest) {
    return std::make_shared<JumpInst>(_dest);
}

void JumpInst::setTarget(BlockPtr _dest) {
    this->dest = _dest;
}

std::string JumpInst::tollvmIR() {
    std::stringstream ss;
    ss << "br label %Block_" << dest->getBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     BranchInst Implementation
//===-----------------------------------------------------------===//

BranchInst::BranchInst(BaseValuePtr _cond, BlockPtr _br1, BlockPtr _br2)
    : cond(_cond), iftrue(_br1), iffalse(_br2) {
    assert(_cond->getBaseType()->checkType(BOOL));
}

BranchInstPtr BranchInst::CreatePtr(BaseValuePtr _cond, BlockPtr _br1, BlockPtr _br2) {
    return std::make_shared<BranchInst>(_cond, _br1, _br2);
}


void BranchInst::setTrueTarget(BlockPtr _iftrue) {
    this->iftrue = _iftrue;
}

void BranchInst::setFalseTarget(BlockPtr _iffalse) {
    this->iffalse = _iffalse;
}

std::string BranchInst::tollvmIR() {
    std::stringstream ss;
    ss << "br i1 " << cond->tollvmIR() << ", ";
    ss << "label %Block_" << iftrue->getBlockIdx() << ", ";
    ss << "label %Block_" << iffalse->getBlockIdx();
    return ss.str();
}
