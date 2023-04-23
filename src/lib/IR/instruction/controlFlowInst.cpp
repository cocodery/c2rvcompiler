#include "controlFlowInst.hh"

//===-----------------------------------------------------------===//
//                     ReturnInst Implementation
//===-----------------------------------------------------------===//

ReturnInst::ReturnInst(ScalarTypePtr _type, BaseValuePtr _value) : ret_type(_type), ret_value(_value) {
    if (ret_type->VoidType()) {
        assert(_value == nullptr);
    } else {
        assert(ret_type->getAttrType() == ret_value->getBaseType()->getAttrType());
        assert(ret_value->IsOprand());
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
//                     JumpInst Implementation
//===-----------------------------------------------------------===//

JumpInst::JumpInst(CfgNodePtr _block, CfgNodePtr _dest) : block(_block), dest(_dest) {
    assert(block != nullptr);
    if (dest != nullptr) {
        block->AddSuccessor(dest);
        dest->AddPredcessor(block);
    }
}

JumpInstPtr JumpInst::CreatePtr(CfgNodePtr _block, CfgNodePtr _dest) {
    return std::make_shared<JumpInst>(_block, _dest);
}

void JumpInst::setTarget(CfgNodePtr _dest) {
    assert(dest == nullptr);
    dest = _dest;
    block->AddSuccessor(dest);
    dest->AddPredcessor(block);
}

std::string JumpInst::tollvmIR() {
    std::stringstream ss;
    ss << "br label %Block_" << dest->getBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     BranchInst Implementation
//===-----------------------------------------------------------===//

BranchInst::BranchInst(CfgNodePtr _block, BaseValuePtr _cond, CfgNodePtr _br1, CfgNodePtr _br2)
    : block(_block), cond(_cond), iftrue(_br1), iffalse(_br2) {
    assert(cond->getBaseType()->BoolType());
    assert(block != nullptr);
    if (iftrue != nullptr) {
        block->AddSuccessor(iftrue);
        iftrue->AddPredcessor(block);
    }
    if (iffalse != nullptr) {
        block->AddSuccessor(iffalse);
        iffalse->AddPredcessor(block);
    }
}

BranchInstPtr BranchInst::CreatePtr(CfgNodePtr _block, BaseValuePtr _cond, CfgNodePtr _br1, CfgNodePtr _br2) {
    return std::make_shared<BranchInst>(_block, _cond, _br1, _br2);
}

void BranchInst::setTrueTarget(CfgNodePtr _iftrue) {
    assert(iftrue == nullptr);
    iftrue = _iftrue;
    block->AddSuccessor(iftrue);
    iftrue->AddPredcessor(block);
}

void BranchInst::setFalseTarget(CfgNodePtr _iffalse) {
    assert(iffalse == nullptr);
    iffalse = _iffalse;
    block->AddSuccessor(iffalse);
    iffalse->AddPredcessor(block);
}

std::string BranchInst::tollvmIR() {
    std::stringstream ss;
    ss << "br i1 " << cond->tollvmIR() << ", ";
    ss << "label %Block_" << iftrue->getBlockIdx() << ", ";
    ss << "label %Block_" << iffalse->getBlockIdx();
    return ss.str();
}
