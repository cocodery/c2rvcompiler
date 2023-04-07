#include "memoryInst.hh"

//===-----------------------------------------------------------===//
//                     AllocaInst Implementation
//===-----------------------------------------------------------===//

AllocaInst::AllocaInst(BaseTypePtr _ty_stored, BaseValuePtr _addr) 
    : type_stored(_ty_stored), addr_alloca(_addr) {
    BaseTypePtr type_alloca = addr_alloca->getBaseType();
    assert(type_stored->getAttrType() == type_alloca->getAttrType());
    assert(type_stored->IsNotPtr()    && type_alloca->IsPointer());
}

AllocaInstPtr AllocaInst::CreatePtr(BaseTypePtr _ty_stored, BaseValuePtr _addr) {
    return std::make_shared<AllocaInst>(_ty_stored, _addr);
}

BaseValuePtr AllocaInst::AllocaAddress(BaseTypePtr _ty_stored, BaseTypePtr _ty_alloca, BlockPtr block) {
    BaseValuePtr _addr = Variable::CreatePtr(_ty_alloca);
    block->insertInst(CreatePtr(_ty_stored, _addr));
    return _addr;
}

std::string AllocaInst::tollvmIR() {
    std::stringstream ss;
    ss << addr_alloca->tollvmIR() << " = alloca " << type_stored->tollvmIR() << ", align 4";
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     StoreInst Implementation
//===-----------------------------------------------------------===//

StoreInst::StoreInst(BaseValuePtr addr, BaseValuePtr value)
    : store_addr(addr), store_value(value) {
    BaseTypePtr type_addr  = store_addr ->getBaseType();
    BaseTypePtr type_value = store_value->getBaseType();
    assert(type_addr->getAttrType() == type_value->getAttrType());
    assert(type_addr->IsPointer()   && type_value->IsNotPtr());
    assert(type_addr->IsScalar()    && type_value->IsScalar());
}

StoreInstPtr StoreInst::CreatePtr(BaseValuePtr addr, BaseValuePtr value) {
    return std::make_shared<StoreInst>(addr, value);
}

void StoreInst::StoreValue2Mem(BaseValuePtr addr, BaseValuePtr value, BlockPtr block) {
    // for store, only two target type, `INT` and `FLOAT`
    if (value->getBaseType()->IsPointer()) {
        value = LoadInst::LoadValuefromMem(value, block);
    }

    // BaseValuePtr convertee = scalarTypeConvert(addr->getBaseType()->getAttrType(), value, block);
    block->insertInst(CreatePtr(addr, value));
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
    BaseTypePtr type_addr  = load_addr ->getBaseType();
    BaseTypePtr type_value = load_value->getBaseType();
    assert(type_addr->getAttrType() == type_value->getAttrType());
    assert(type_addr->IsPointer()   && type_value->IsNotPtr());
    assert(type_addr->IsScalar()    && type_value->IsScalar());
}

LoadInstPtr LoadInst::CreatePtr(BaseValuePtr value, BaseValuePtr addr) {
    return std::make_shared<LoadInst>(value, addr);
}

BaseValuePtr LoadInst::LoadValuefromMem(BaseValuePtr addr, BlockPtr block) {
    ATTR_TYPE attr_type = addr->getBaseType()->getAttrType();
    BaseValuePtr value = Variable::CreatePtr(ScalarType::CreatePtr(attr_type, MUTABLE, NOTPTR, SCALAR, LOCAL));
    block->insertInst(CreatePtr(value, addr));
    return value;
}

std::string LoadInst::tollvmIR() {
    std::stringstream ss;
    ss << load_value->tollvmIR() << " = load " << load_value->getBaseType()->tollvmIR();
    ss << ", " << load_addr->getBaseType()->tollvmIR() << ' ' << load_addr->tollvmIR();
    ss << ", align 4";
    return ss.str();
}

GetElementPtrInst::GetElementPtrInst(BaseValuePtr _ptr, BaseTypePtr _type, BaseValuePtr _addr, BaseValuePtr _off)
    : target_ptr(_ptr), base_type(_type), base_addr(_addr), offset(_off) {
    assert(target_ptr->getBaseType()->getAttrType() == base_addr->getBaseType()->getAttrType());
    assert(base_type->isArray());
    assert(base_addr->getBaseType()->isArray());
    ListTypePtr list1 = std::static_pointer_cast<ListType>(_type);
    ListTypePtr list2 = std::static_pointer_cast<ListType>(base_addr->getBaseType());
    assert(list1->getArrDims() == list2->getArrDims());
}

GepInstPtr GetElementPtrInst::CreatePtr(BaseValuePtr _ptr, BaseTypePtr _type, BaseValuePtr _addr, BaseValuePtr _off) {
    return std::make_shared<GetElementPtrInst>(_ptr ,_type, _addr, _off);
}

BaseValuePtr GetElementPtrInst::GepFromBaseAddr(BaseTypePtr _type, BaseValuePtr _addr, BaseValuePtr _off, BlockPtr block) {
    BaseValuePtr _ptr = Variable::CreatePtr(ScalarType::CreatePtr(_type->getAttrType(), MUTABLE, POINTER, SCALAR, LOCAL));
    block->insertInst(CreatePtr(_ptr, _type, _addr, _off));
    return _ptr;
}

std::string GetElementPtrInst::tollvmIR() {
    std::stringstream ss;
    ss << target_ptr->tollvmIR() << " = getelementptr inbounds " << base_type->tollvmIR();
    ss << ", " << base_addr->getBaseType()->tollvmIR() << ' ' << base_addr->tollvmIR();
    ss << ", i32 0, " << offset->getBaseType()->tollvmIR() << ' ' << offset->tollvmIR();
    return ss.str();
}
