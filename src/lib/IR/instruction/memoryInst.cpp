#include "memoryInst.hh"

AllocaInst::AllocaInst(TypeID _tid, BaseValuePtr value) 
    : alloca_type(ScalarType::CreatePtr(_tid)), alloca_addr(value) {
}

AllocaInst::AllocaInst(ListTypePtr list_type, BaseValuePtr value) 
    : alloca_type(list_type), alloca_addr(value) {

}

AllocaInstPtr AllocaInst::CreatePtr(TypeID _tid, BaseValuePtr value) {
    return std::make_shared<AllocaInst>(_tid, value);
}

AllocaInstPtr AllocaInst::CreatePtr(ListTypePtr list_type, BaseValuePtr value) {
    return std::make_shared<AllocaInst>(list_type, value);
}

std::string AllocaInst::toString() {
    return "alloca";
}

std::string AllocaInst::tollvmIR() {
    return "alloca";
}
