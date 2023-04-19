#include "typeconvert.hh"

//===-----------------------------------------------------------===//
//                     SitoFpInst Implementation
//===-----------------------------------------------------------===//

SitoFpInst::SitoFpInst(VariablePtr _value1, BaseValuePtr _value2) : fp_value(_value1), si_value(_value2) {
    assert(si_value->getBaseType()->IntType() || si_value->getBaseType()->BoolType());
    assert(si_value->IsOprand());
}

SitoFpInstPtr SitoFpInst::CreatePtr(VariablePtr _value1, BaseValuePtr _value2) {
    return std::make_shared<SitoFpInst>(_value1, _value2);
}

VariablePtr SitoFpInst::DoSitoFp(BaseValuePtr _si, BlockPtr block) {
    VariablePtr _fp = Variable::CreatePtr(type_float_L);
    block->insertInst(CreatePtr(_fp, _si));
    return _fp;
}

std::string SitoFpInst::tollvmIR() {
    std::stringstream ss;
    ss << fp_value->tollvmIR() << " = sitofp " << si_value->getBaseType()->tollvmIR();
    ss << ' ' << si_value->tollvmIR() << " to " << fp_value->getBaseType()->tollvmIR();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     FptoSiInst Implementation
//===-----------------------------------------------------------===//

FptoSiInst::FptoSiInst(VariablePtr _value1, BaseValuePtr _value2) : si_value(_value1), fp_value(_value2) {
    assert(fp_value->getBaseType()->FloatType());
    assert(fp_value->IsOprand());
}

FptoSiInstPtr FptoSiInst::CreatePtr(VariablePtr _value1, BaseValuePtr _value2) {
    return std::make_shared<FptoSiInst>(_value1, _value2);
}

VariablePtr FptoSiInst::DoFptoSi(ATTR_TYPE _type, BaseValuePtr _fp, BlockPtr block) {
    assert(_type == BOOL || _type == INT);
    VariablePtr _si = Variable::CreatePtr((_type == INT) ? type_int_L : type_bool);
    block->insertInst(CreatePtr(_si, _fp));
    return _si;
}

std::string FptoSiInst::tollvmIR() {
    std::stringstream ss;
    ss << si_value->tollvmIR() << " = fptosi " << fp_value->getBaseType()->tollvmIR();
    ss << ' ' << fp_value->tollvmIR() << " to " << si_value->getBaseType()->tollvmIR();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     ZextInst Implementation
//===-----------------------------------------------------------===//

ZextInst::ZextInst(VariablePtr _value1, BaseValuePtr _value2) : l_value(_value1), s_value(_value2) {
    assert(s_value->getBaseType()->BoolType());
    assert(s_value->IsOprand());
}

ZextInstPtr ZextInst::CreatePtr(VariablePtr _value1, BaseValuePtr _value2) {
    return std::make_shared<ZextInst>(_value1, _value2);
}

VariablePtr ZextInst::DoZeroExt(BaseValuePtr _sv, BlockPtr block) {
    VariablePtr _lv = Variable::CreatePtr(type_int_L);
    block->insertInst(CreatePtr(_lv, _sv));
    return _lv;
}

std::string ZextInst::tollvmIR() {
    std::stringstream ss;
    ss << l_value->tollvmIR() << " = zext " << s_value->getBaseType()->tollvmIR();
    ss << ' ' << s_value->tollvmIR() << " to " << l_value->getBaseType()->tollvmIR();
    return ss.str();
}
