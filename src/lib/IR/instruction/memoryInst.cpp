#include "memoryInst.hh"

//===-----------------------------------------------------------===//
//                     AllocaInst Implementation
//===-----------------------------------------------------------===//

AllocaInst::AllocaInst(TypeID _tid, BaseValuePtr value) 
    : alloca_type(ScalarType::CreatePtr(_tid)), alloca_addr(value) {
    // create alloca_addr and alloca_inst sequentially
    // so don't need to check for alloca_inst
}

AllocaInst::AllocaInst(ListTypePtr list_type, BaseValuePtr value) 
    : alloca_type(list_type), alloca_addr(value) {
    // create alloca_addr and alloca_inst sequentially
    // so don't need to check for alloca_inst
}

AllocaInstPtr AllocaInst::CreatePtr(TypeID _tid, BaseValuePtr value) {
    return std::make_shared<AllocaInst>(_tid, value);
}

AllocaInstPtr AllocaInst::CreatePtr(ListTypePtr list_type, BaseValuePtr value) {
    return std::make_shared<AllocaInst>(list_type, value);
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

std::string StoreInst::tollvmIR() {
    std::stringstream ss;
    ss << "store " << store_value->getBaseType()->tollvmIR() << ' ' << store_value->tollvmIR();
    ss << ", "     << store_addr ->getBaseType()->tollvmIR() << ' ' << store_addr ->tollvmIR();
    ss << ", align 4";
    return ss.str();
}
