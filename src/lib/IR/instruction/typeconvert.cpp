#include "typeconvert.hh"

//===-----------------------------------------------------------===//
//                     SitoFpInst Implementation
//===-----------------------------------------------------------===//

SitoFpInst::SitoFpInst(VariablePtr _value1, BaseValuePtr _value2)  
    : fp_value(_value1), si_value(_value2) {
    assert(fp_value->getBaseType()->floatType());
    assert(si_value->getBaseType()->intType() || si_value->getBaseType()->boolType());
}

SitoFpInstPtr SitoFpInst::CreatePtr(VariablePtr _value1, BaseValuePtr _value2) {
    return std::make_shared<SitoFpInst>(_value1, _value2);
}

VariablePtr SitoFpInst::DoSitoFp(BaseValuePtr _si, BlockPtr block) {
    VariablePtr _fp = Variable::CreatePtr(ScalarType::CreatePtr(FLOAT, MUTABLE, NOTPTR, SCALAR, LOCAL));
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

FptoSiInst::FptoSiInst(VariablePtr _value1, BaseValuePtr _value2)
    : si_value(_value1), fp_value(_value2) {
    assert(si_value->getBaseType()->intType() || si_value->getBaseType()->boolType());
    assert(fp_value->getBaseType()->floatType());
}

FptoSiInstPtr FptoSiInst::CreatePtr(VariablePtr _value1, BaseValuePtr _value2) {
    return std::make_shared<FptoSiInst>(_value1, _value2);
}

VariablePtr FptoSiInst::DoFptoSi(ATTR_TYPE _type, BaseValuePtr _fp, BlockPtr block) {
    VariablePtr _si = Variable::CreatePtr(ScalarType::CreatePtr(_type, MUTABLE, NOTPTR, SCALAR, LOCAL));
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

ZextInst::ZextInst(VariablePtr _value1, BaseValuePtr _value2)
    : l_value(_value1), s_value(_value2) {
    assert(_value1->getBaseType()->intType());
    assert(_value2->getBaseType()->boolType());
}

ZextInstPtr ZextInst::CreatePtr(VariablePtr _value1, BaseValuePtr _value2) {
    return std::make_shared<ZextInst>(_value1, _value2);
}

VariablePtr ZextInst::DoZeroExt(BaseValuePtr _sv, BlockPtr block) {
    VariablePtr _lv = Variable::CreatePtr(ScalarType::CreatePtr(INT, MUTABLE, NOTPTR, SCALAR, LOCAL));
    block->insertInst(CreatePtr(_lv, _sv));
    return _lv;
}

std::string ZextInst::tollvmIR() {
    std::stringstream ss;
    ss << l_value->tollvmIR() << " = zext " << s_value->getBaseType()->tollvmIR();
    ss << ' ' << s_value->tollvmIR() << " to " << l_value->getBaseType()->tollvmIR();
    return ss.str();
}
