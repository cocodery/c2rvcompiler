#include "typeconvert.hh"

//===-----------------------------------------------------------===//
//                     SitoFpInst Implementation
//===-----------------------------------------------------------===//

SitoFpInst::SitoFpInst(BaseValuePtr _value1, BaseValuePtr _value2)  
    : fp_value(_value1), si_value(_value2) {
    assert(_value1->getBaseType()->checkType(FLOAT));
    assert(_value2->getBaseType()->checkType(BOOL | INT));
}

SitoFpInstPtr SitoFpInst::CreatePtr(BaseValuePtr _value1, BaseValuePtr _value2) {
    return std::make_shared<SitoFpInst>(_value1, _value2);
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

FptoSiInst::FptoSiInst(BaseValuePtr _value1, BaseValuePtr _value2)
    : si_value(_value1), fp_value(_value2) {
    assert(_value1->getBaseType()->checkType(BOOL | INT));
    assert(_value2->getBaseType()->checkType(FLOAT));
}

FptoSiInstPtr FptoSiInst::CreatePtr(BaseValuePtr _value1, BaseValuePtr _value2) {
    return std::make_shared<FptoSiInst>(_value1, _value2);
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

ZextInst::ZextInst(BaseValuePtr _value1, BaseValuePtr _value2)
    : l_value(_value1), s_value(_value2) {
    assert(_value1->getBaseType()->checkType(INT));
    assert(_value2->getBaseType()->checkType(BOOL));
}

ZextInstPtr ZextInst::CreatePtr(BaseValuePtr _value1, BaseValuePtr _value2) {
    return std::make_shared<ZextInst>(_value1, _value2);
}

std::string ZextInst::tollvmIR() {
    std::stringstream ss;
    ss << l_value->tollvmIR() << " = zext " << s_value->getBaseType()->tollvmIR();
    ss << ' ' << s_value->tollvmIR() << " to " << l_value->getBaseType()->tollvmIR();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     scalarTypeConvert Implementation
//===-----------------------------------------------------------===//

BaseValuePtr scalarTypeConvert(TypeID tid, BaseValuePtr convertee, BlockPtr block) {
    // if ret_type is `VOID`, return nullpre directly
    if (tid == VOID) {
        return nullptr;
    }
    BaseTypePtr type_convertee = convertee->getBaseType();
    TypeID tid_convertee = type_convertee->getMaskedType(BOOL | INT | FLOAT);
    // if tid == tid_convertee, no need to convert
    if (tid == tid_convertee) {
        return convertee;
    }
    // make sure target type is `INT` or `FLOAT`
    assert((tid == INT) || (tid == FLOAT));
    // if convertee is `CONSTANT`, use `fixType` to convert
    if (type_convertee->checkType(INT | FLOAT | BOOL, CONSTANT)) {
        ConstantPtr constant_convertee = std::static_pointer_cast<Constant>(convertee);
        ConstantPtr constant = Constant::CreatePtr(ScalarType::CreatePtr(tid | CONSTANT), constant_convertee->getValue());
        constant->fixValue(tid);
        return constant;
    }
    // use instruction to convert
    BaseValuePtr convert_value = Variable::CreatePtr(ScalarType::CreatePtr(tid | VARIABLE));
    if (tid == FLOAT) {
        // convert i1 or i32 to float
        InstPtr sitofp_inst = SitoFpInst::CreatePtr(convert_value, convertee);
        block->insertInst(sitofp_inst);
    } else if (tid == INT) {
        if (tid_convertee == FLOAT) {
            // convert float to i32
            InstPtr fptosi_inst = FptoSiInst::CreatePtr(convert_value, convertee);
            block->insertInst(fptosi_inst);
        } else if (tid_convertee == BOOL) {
            // convert i32 to i32
            InstPtr zext_inst = ZextInst::CreatePtr(convert_value, convertee);
            block->insertInst(zext_inst);
        }
    }
    return convert_value;
}