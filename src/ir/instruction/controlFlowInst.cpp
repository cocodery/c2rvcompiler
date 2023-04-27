#include "controlFlowInst.hh"

//===-----------------------------------------------------------===//
//                     ReturnInst Implementation
//===-----------------------------------------------------------===//

ReturnInst::ReturnInst(ScalarTypePtr _type, BaseValuePtr _value, CfgNodePtr block)
    : ret_type(_type), ret_value(_value), Instruction(block) {
    if (ret_type->VoidType()) {
        assert(_value == nullptr);
    } else {
        assert(ret_type->getAttrType() == ret_value->getBaseType()->getAttrType());
        assert(ret_value->IsOprand());
    }
}

RetInstPtr ReturnInst::CreatePtr(ScalarTypePtr _type, BaseValuePtr _value, CfgNodePtr block) {
    return std::make_shared<ReturnInst>(_type, _value, block);
}

std::string ReturnInst::tollvmIR() {
    std::stringstream ss;
    if (ret_type->VoidType()) {
        ss << "ret void";
    } else {
        ss << "ret " << ret_type->tollvmIR() << ' ' << ret_value->tollvmIR();
    }
    ss << "; " << parent->GetBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     JumpInst Implementation
//===-----------------------------------------------------------===//

JumpInst::JumpInst(CfgNodePtr _dest, CfgNodePtr block) : dest(_dest), Instruction(block) {
    assert(block != nullptr);
    if (dest != nullptr) {
        parent->AddSuccessor(dest);
        dest->AddPredcessor(parent);
    }
}

JumpInstPtr JumpInst::CreatePtr(CfgNodePtr _dest, CfgNodePtr _block) {
    return std::make_shared<JumpInst>(_dest, _block);
}

void JumpInst::setTarget(CfgNodePtr _dest) {
    assert(dest == nullptr);
    dest = _dest;
    parent->AddSuccessor(dest);
    dest->AddPredcessor(parent);
}

std::string JumpInst::tollvmIR() {
    std::stringstream ss;
    ss << "br label %Block_" << dest->GetBlockIdx();
    ss << "; " << parent->GetBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     BranchInst Implementation
//===-----------------------------------------------------------===//

BranchInst::BranchInst(BaseValuePtr _cond, CfgNodePtr _br1, CfgNodePtr _br2, CfgNodePtr block)
    : cond(_cond), iftrue(_br1), iffalse(_br2), Instruction(block) {
    assert(cond->getBaseType()->BoolType());
    assert(parent != nullptr);
    if (iftrue != nullptr) {
        parent->AddSuccessor(iftrue);
        iftrue->AddPredcessor(parent);
    }
    if (iffalse != nullptr) {
        parent->AddSuccessor(iffalse);
        iffalse->AddPredcessor(parent);
    }
}

BranchInstPtr BranchInst::CreatePtr(BaseValuePtr _cond, CfgNodePtr _br1, CfgNodePtr _br2, CfgNodePtr block) {
    return std::make_shared<BranchInst>(_cond, _br1, _br2, block);
}

void BranchInst::setTrueTarget(CfgNodePtr _iftrue) {
    assert(iftrue == nullptr);
    iftrue = _iftrue;
    parent->AddSuccessor(iftrue);
    iftrue->AddPredcessor(parent);
}

void BranchInst::setFalseTarget(CfgNodePtr _iffalse) {
    assert(iffalse == nullptr);
    iffalse = _iffalse;
    parent->AddSuccessor(iffalse);
    iffalse->AddPredcessor(parent);
}

std::string BranchInst::tollvmIR() {
    std::stringstream ss;
    ss << "br i1 " << cond->tollvmIR() << ", ";
    ss << "label %Block_" << iftrue->GetBlockIdx() << ", ";
    ss << "label %Block_" << iffalse->GetBlockIdx();
    ss << "; " << parent->GetBlockIdx();
    return ss.str();
}
