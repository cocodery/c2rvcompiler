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

JumpInst::JumpInst(BlockPtr _dest) : dest(_dest) {}

JumpInstPtr JumpInst::CreatePtr(BlockPtr _dest) { return std::make_shared<JumpInst>(_dest); }

void JumpInst::setTarget(BlockPtr _dest) { this->dest = _dest; }

std::string JumpInst::tollvmIR() {
    std::stringstream ss;
    ss << "br label %Block_" << dest->getBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     BranchInst Implementation
//===-----------------------------------------------------------===//

BranchInst::BranchInst(BaseValuePtr _cond, BlockPtr _br1, BlockPtr _br2) : cond(_cond), iftrue(_br1), iffalse(_br2) {
    assert(cond->getBaseType()->BoolType());
}

BranchInstPtr BranchInst::CreatePtr(BaseValuePtr _cond, BlockPtr _br1, BlockPtr _br2) {
    return std::make_shared<BranchInst>(_cond, _br1, _br2);
}

void BranchInst::setTrueTarget(BlockPtr _iftrue) { this->iftrue = _iftrue; }

void BranchInst::setFalseTarget(BlockPtr _iffalse) { this->iffalse = _iffalse; }

std::string BranchInst::tollvmIR() {
    std::stringstream ss;
    ss << "br i1 " << cond->tollvmIR() << ", ";
    ss << "label %Block_" << iftrue->getBlockIdx() << ", ";
    ss << "label %Block_" << iffalse->getBlockIdx();
    return ss.str();
}
