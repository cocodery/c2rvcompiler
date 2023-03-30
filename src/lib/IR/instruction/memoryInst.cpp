#include "memoryInst.hh"

//===-----------------------------------------------------------===//
//                     AllocaInst Implementation
//===-----------------------------------------------------------===//

AllocaInst::AllocaInst(BaseTypePtr _type, BaseValuePtr _value) 
    : alloca_type(_type), alloca_addr(_value) {
    // create alloca_addr and alloca_inst sequentially
    // so don't need to check for alloca_inst
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
    return StoreInst::CreatePtr(addr, value);
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

std::string LoadInst::tollvmIR() {
    std::stringstream ss;
    // %20 = load i32, i32* @f, align 4
    ss << load_value->tollvmIR() << " = load " << load_value->getBaseType()->tollvmIR();
    ss << ", " << load_addr->getBaseType()->tollvmIR() << ' ' << load_addr->tollvmIR();
    ss << ", align 4";
    return ss.str();
}
