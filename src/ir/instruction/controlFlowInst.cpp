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
    auto &&inst = std::make_shared<ReturnInst>(_type, _value, block);
    if (_value != nullptr) _value->InsertUser(inst);
    return inst;
}

bool ReturnInst::ReplaceSRC(BaseValuePtr replacee, BaseValuePtr replacer) {
    if (replacee == ret_value) {
        ret_value = replacer;
        return true;
    }
    return false;
}

const BaseValueList ReturnInst::UsedValue() {
    BaseValueList valuelist = BaseValueList();
    if (!ret_type->VoidType()) valuelist.push_back(ret_value);
    return valuelist;
}

std::string ReturnInst::tollvmIR() {
    std::stringstream ss;
    if (ret_type->VoidType()) {
        ss << "ret void";
    } else {
        ss << "ret " << ret_type->tollvmIR() << ' ' << ret_value->tollvmIR();
    }
    ss << "; Inst_" << GetInstIdx() << " from Block_" << parent->GetBlockIdx();
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

bool JumpInst::ReplaceSRC(BaseValuePtr replacee, BaseValuePtr replacer) { return false; }

const BaseValueList JumpInst::UsedValue() { return BaseValueList(); }

std::string JumpInst::tollvmIR() {
    std::stringstream ss;
    ss << "br label %Block_" << dest->GetBlockIdx();
    ss << "; Inst_" << GetInstIdx() << " from Block_" << parent->GetBlockIdx();
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
    auto &&inst = std::make_shared<BranchInst>(_cond, _br1, _br2, block);
    _cond->InsertUser(inst);
    return inst;
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

bool BranchInst::ReplaceSRC(BaseValuePtr replacee, BaseValuePtr replacer) {
    if (replacee == cond) {
        cond = replacer;
        return true;
    }
    return false;
}

const BaseValueList BranchInst::UsedValue() { return BaseValueList({cond}); }

std::string BranchInst::tollvmIR() {
    std::stringstream ss;
    ss << "br i1 " << cond->tollvmIR() << ", ";
    ss << "label %Block_" << iftrue->GetBlockIdx() << ", ";
    ss << "label %Block_" << iffalse->GetBlockIdx();
    ss << "; Inst_" << GetInstIdx() << " from Block_" << parent->GetBlockIdx();
    return ss.str();
}
