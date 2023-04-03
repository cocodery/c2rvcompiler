#include "memoryInst.hh"

//===-----------------------------------------------------------===//
//                     AllocaInst Implementation
//===-----------------------------------------------------------===//

AllocaInst::AllocaInst(BaseTypePtr _type, BaseValuePtr _value) 
    : alloca_type(_type), alloca_addr(_value) {
    TypeID id_type = _type ->getMaskedType(INT | FLOAT, ARRAY);
    TypeID id_addr = _value->getBaseType()->getMaskedType(INT | FLOAT, ARRAY, POINTER);
    assert((id_type | POINTER) == id_addr);
}

AllocaInstPtr AllocaInst::CreatePtr(BaseTypePtr _type, BaseValuePtr _value) {
    return std::make_shared<AllocaInst>(_type, _value);
}

std::string AllocaInst::tollvmIR() {
    std::stringstream ss;
    ss << alloca_addr->tollvmIR() << " = alloca " << alloca_type->tollvmIR() << ", align 4";
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     StoreInst Implementation
//===-----------------------------------------------------------===//

StoreInst::StoreInst(BaseValuePtr addr, BaseValuePtr value)
    : store_addr(addr), store_value(value) {
    TypeID type_addr  = addr ->getBaseType()->getMaskedType(INT | FLOAT | POINTER);
    TypeID type_value = value->getBaseType()->getMaskedType(INT | FLOAT | POINTER);
    assert(type_addr == (type_value | POINTER));
}

StoreInstPtr StoreInst::CreatePtr(BaseValuePtr addr, BaseValuePtr value) {
    return std::make_shared<StoreInst>(addr, value);
}

StoreInstPtr StoreInst::StoreValue2Mem(BaseValuePtr addr, BaseValuePtr value, BlockPtr block) {
    // for store, only two target type, `INT` and `FLOAT`
    BaseValuePtr convertee = scalarTypeConvert(addr->getBaseType()->getMaskedType(INT | FLOAT), value, block);
    return StoreInst::CreatePtr(addr, convertee);
}

std::string StoreInst::tollvmIR() {
    std::stringstream ss;
    ss << "store " << store_value->getBaseType()->tollvmIR() << ' ' << store_value->tollvmIR();
    ss << ", "     << store_addr ->getBaseType()->tollvmIR() << ' ' << store_addr ->tollvmIR();
    ss << ", align 4";
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     LoadInst Implementation
//===-----------------------------------------------------------===//

LoadInst::LoadInst(BaseValuePtr value, BaseValuePtr addr)
    : load_value(value), load_addr(addr) {
    TypeID type_value = value->getBaseType()->getMaskedType(INT | FLOAT | POINTER);
    TypeID type_addr  = addr ->getBaseType()->getMaskedType(INT | FLOAT | POINTER);
    assert((type_value | POINTER) == type_addr);
}

LoadInstPtr LoadInst::CreatePtr(BaseValuePtr value, BaseValuePtr addr) {
    return std::make_shared<LoadInst>(value, addr);
}

BaseValuePtr LoadInst::LoadValuefromMem(BaseValuePtr addr, BlockPtr block) {
    TypeID value_tid = addr->getBaseType()->getMaskedType(INT | FLOAT);
    BaseValuePtr value = Variable::CreatePtr(ScalarType::CreatePtr(value_tid | VARIABLE));
    LoadInstPtr load_inst = CreatePtr(value, addr);
    block->insertInst(load_inst);
    return value;
}

std::string LoadInst::tollvmIR() {
    std::stringstream ss;
    ss << load_value->tollvmIR() << " = load " << load_value->getBaseType()->tollvmIR();
    ss << ", " << load_addr->getBaseType()->tollvmIR() << ' ' << load_addr->tollvmIR();
    ss << ", align 4";
    return ss.str();
}
