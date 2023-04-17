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

VariablePtr AllocaInst::DoAllocaAddr(BaseTypePtr _ty_stored, BaseTypePtr _ty_alloca, BlockPtr block) {
    VariablePtr _addr = Variable::CreatePtr(_ty_alloca);
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

void StoreInst::DoStoreValue(BaseValuePtr addr, BaseValuePtr value, BlockPtr block) {
    // for store, only two target type, `INT` and `FLOAT`
    if (value->getBaseType()->IsPointer()) {
        value = LoadInst::DoLoadValue(value, block);
    }
    BaseValuePtr convertee = Value::scalarTypeConvert(addr->getBaseType()->getAttrType(), value, block);
    block->insertInst(CreatePtr(addr, convertee));
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

BaseValuePtr LoadInst::DoLoadValue(BaseValuePtr addr, BlockPtr block) {
    BaseTypePtr addr_type = addr->getBaseType();
    assert(addr_type->IsPointer() && addr_type->IsScalar() && (addr_type->intType() || addr_type->floatType()));
    BaseValuePtr value = Variable::CreatePtr(addr_type->intType() ? type_int_L : type_float_L);
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

//===-----------------------------------------------------------===//
//                     GetElementPtrInst Implementation
//===-----------------------------------------------------------===//

GetElementPtrInst::GetElementPtrInst(BaseValuePtr _ptr, BaseTypePtr _type, BaseValuePtr _addr, OffsetList _off)
    : target_ptr(_ptr), store_type(_type), base_addr(_addr), offset_list(_off) {
    assert(target_ptr->getBaseType()->getAttrType() == store_type->getAttrType());
    assert(target_ptr->getBaseType()->getAttrType() == base_addr->getBaseType()->getAttrType());
    assert(store_type->IsNotPtr() && base_addr->getBaseType()->IsPointer());
    if (store_type->IsScalar()) {
        assert(offset_list.size() == 1);
    } else {
        ListTypePtr list1 = std::static_pointer_cast<ListType>(store_type);
        ListTypePtr list2 = std::static_pointer_cast<ListType>(base_addr->getBaseType());
        assert(list1->getArrSize() == list2->getArrSize());
        assert(offset_list.size() == 2);
    }
}

GepInstPtr GetElementPtrInst::CreatePtr(BaseValuePtr _ptr, BaseTypePtr _type, BaseValuePtr _addr, OffsetList _off) {
    return std::make_shared<GetElementPtrInst>(_ptr ,_type, _addr, _off);
}

BaseValuePtr GetElementPtrInst::DoGetPointer(BaseTypePtr _type, BaseValuePtr _addr, OffsetList _off, BlockPtr block) {
    // only have INT-array or FLOAT-array
    BaseValuePtr _ptr = Variable::CreatePtr(_type->intType() ? type_int_ptr_L : type_float_ptr_L);
    block->insertInst(CreatePtr(_ptr, _type, _addr, _off));
    return _ptr;
}

std::string GetElementPtrInst::tollvmIR() {
    std::stringstream ss;
    ss << target_ptr->tollvmIR() << " = getelementptr inbounds " << store_type->tollvmIR();
    ss << ", " << base_addr->getBaseType()->tollvmIR() << ' ' << base_addr->tollvmIR();
    for (auto &&offset : offset_list) {
        ss << ", " << offset->getBaseType()->tollvmIR() << ' ' << offset->tollvmIR();
    }
    return ss.str();
}
