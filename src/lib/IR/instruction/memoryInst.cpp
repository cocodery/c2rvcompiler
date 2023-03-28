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

std::string AllocaInst::tollvmIR() {
    std::stringstream ss;
    ss << alloca_addr->tollvmIR() << " = alloca " << alloca_type->tollvmIR() << ", align 4";
    return ss.str();
}
