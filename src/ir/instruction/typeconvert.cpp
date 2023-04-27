#include "typeconvert.hh"

//===-----------------------------------------------------------===//
//                     SitoFpInst Implementation
//===-----------------------------------------------------------===//

SitoFpInst::SitoFpInst(VariablePtr _value1, BaseValuePtr _value2, CfgNodePtr block)
    : fp_value(_value1), si_value(_value2), Instruction(block) {
    assert(si_value->getBaseType()->IntType() || si_value->getBaseType()->BoolType());
    assert(si_value->IsOprand());
}

SitoFpInstPtr SitoFpInst::CreatePtr(VariablePtr _value1, BaseValuePtr _value2, CfgNodePtr block) {
    return std::make_shared<SitoFpInst>(_value1, _value2, block);
}

VariablePtr SitoFpInst::DoSitoFp(BaseValuePtr _si, CfgNodePtr block) {
    VariablePtr _fp = Variable::CreatePtr(type_float_L);
    block->InsertInst(CreatePtr(_fp, _si, block));
    return _fp;
}

std::string SitoFpInst::tollvmIR() {
    std::stringstream ss;
    ss << fp_value->tollvmIR() << " = sitofp " << si_value->getBaseType()->tollvmIR();
    ss << ' ' << si_value->tollvmIR() << " to " << fp_value->getBaseType()->tollvmIR();
    ss << "; " << parent->GetBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     FptoSiInst Implementation
//===-----------------------------------------------------------===//

FptoSiInst::FptoSiInst(VariablePtr _value1, BaseValuePtr _value2, CfgNodePtr block)
    : si_value(_value1), fp_value(_value2), Instruction(block) {
    assert(fp_value->getBaseType()->FloatType());
    assert(fp_value->IsOprand());
}

FptoSiInstPtr FptoSiInst::CreatePtr(VariablePtr _value1, BaseValuePtr _value2, CfgNodePtr block) {
    return std::make_shared<FptoSiInst>(_value1, _value2, block);
}

VariablePtr FptoSiInst::DoFptoSi(ATTR_TYPE _type, BaseValuePtr _fp, CfgNodePtr block) {
    assert(_type == BOOL || _type == INT);
    VariablePtr _si = Variable::CreatePtr((_type == INT) ? type_int_L : type_bool);
    block->InsertInst(CreatePtr(_si, _fp, block));
    return _si;
}

std::string FptoSiInst::tollvmIR() {
    std::stringstream ss;
    ss << si_value->tollvmIR() << " = fptosi " << fp_value->getBaseType()->tollvmIR();
    ss << ' ' << fp_value->tollvmIR() << " to " << si_value->getBaseType()->tollvmIR();
    ss << "; " << parent->GetBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     ZextInst Implementation
//===-----------------------------------------------------------===//

ZextInst::ZextInst(VariablePtr _value1, BaseValuePtr _value2, CfgNodePtr block)
    : l_value(_value1), s_value(_value2), Instruction(block) {
    assert(s_value->getBaseType()->BoolType());
    assert(s_value->IsOprand());
}

ZextInstPtr ZextInst::CreatePtr(VariablePtr _value1, BaseValuePtr _value2, CfgNodePtr block) {
    return std::make_shared<ZextInst>(_value1, _value2, block);
}

VariablePtr ZextInst::DoZeroExt(BaseValuePtr _sv, CfgNodePtr block) {
    VariablePtr _lv = Variable::CreatePtr(type_int_L);
    block->InsertInst(CreatePtr(_lv, _sv, block));
    return _lv;
}

std::string ZextInst::tollvmIR() {
    std::stringstream ss;
    ss << l_value->tollvmIR() << " = zext " << s_value->getBaseType()->tollvmIR();
    ss << ' ' << s_value->tollvmIR() << " to " << l_value->getBaseType()->tollvmIR();
    ss << "; " << parent->GetBlockIdx();
    return ss.str();
}
