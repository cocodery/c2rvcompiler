#include "memoryInst.hh"

//===-----------------------------------------------------------===//
//                     AllocaInst Implementation
//===-----------------------------------------------------------===//

AllocaInst::AllocaInst(BaseTypePtr _ty_stored, BaseValuePtr _addr, CfgNodePtr block)
    : type_stored(_ty_stored), addr_alloca(_addr), Instruction(block) {
    BaseTypePtr type_alloca = addr_alloca->getBaseType();
    assert(type_stored->IntType() || type_stored->FloatType());
    assert(type_stored->getAttrType() == type_alloca->getAttrType());
    assert(type_stored->IsNotPtr() && type_alloca->IsPointer());
}

AllocaInstPtr AllocaInst::CreatePtr(BaseTypePtr _ty_stored, BaseValuePtr _addr, CfgNodePtr block) {
    return std::make_shared<AllocaInst>(_ty_stored, _addr, block);
}

VariablePtr AllocaInst::DoAllocaAddr(BaseTypePtr _ty_stored, BaseTypePtr _ty_alloca, CfgNodePtr block) {
    VariablePtr _addr = Variable::CreatePtr(_ty_alloca, nullptr);
    auto &&inst = CreatePtr(_ty_stored, _addr, block);
    _addr->SetParent(inst);
    block->InsertInstBack(inst);
    return _addr;
}

bool AllocaInst::IsAllocaInst() const { return true; }
const BaseTypePtr AllocaInst::GetAllocaType() const { return type_stored; }
const BaseValuePtr AllocaInst::GetAllocaAddr() const { return addr_alloca; }

std::string AllocaInst::tollvmIR() {
    std::stringstream ss;
    ss << addr_alloca->tollvmIR() << " = alloca " << type_stored->tollvmIR() << ", align 4";
    ss << "; Inst_" << GetInstIdx() << " from Block_" << parent->GetBlockIdx();

    // cout << addr_alloca->tollvmIR() << " use-list :" << endl;
    // for (auto &&inst : addr_alloca->GetUserList()) {
    //     cout << inst->tollvmIR() << endl;
    // }
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     StoreInst Implementation
//===-----------------------------------------------------------===//

StoreInst::StoreInst(BaseValuePtr addr, BaseValuePtr value, CfgNodePtr block)
    : store_addr(addr), store_value(value), Instruction(block) {
    BaseTypePtr type_addr = store_addr->getBaseType();
    BaseTypePtr type_value = store_value->getBaseType();
    assert(type_addr->getAttrType() == type_value->getAttrType());
    assert(type_addr->IsPointer() && type_value->IsNotPtr());
    assert(type_addr->IsScalar() && type_value->IsScalar());
}

StoreInstPtr StoreInst::CreatePtr(BaseValuePtr addr, BaseValuePtr value, CfgNodePtr block) {
    return std::make_shared<StoreInst>(addr, value, block);
}

void StoreInst::DoStoreValue(BaseValuePtr addr, BaseValuePtr value, CfgNodePtr block) {
    // for store, only two target type, `INT` and `FLOAT`
    assert(value->IsOprand());
    BaseValuePtr convertee = Value::ScalarTypeConvert(addr->getBaseType()->getAttrType(), value, block);
    auto &&inst = CreatePtr(addr, convertee, block);
    addr->InsertUser(inst);
    convertee->InsertUser(inst);
    block->InsertInstBack(inst);
}

bool StoreInst::IsStoreInst() const { return true; }
const BaseValuePtr StoreInst::GetStoreAddr() const { return store_addr; }

std::string StoreInst::tollvmIR() {
    std::stringstream ss;
    ss << "store " << store_value->getBaseType()->tollvmIR() << ' ' << store_value->tollvmIR();
    ss << ", " << store_addr->getBaseType()->tollvmIR() << ' ' << store_addr->tollvmIR();
    ss << ", align 4";
    ss << "; Inst_" << GetInstIdx() << " from Block_" << parent->GetBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     LoadInst Implementation
//===-----------------------------------------------------------===//

LoadInst::LoadInst(VariablePtr value, BaseValuePtr addr, CfgNodePtr block)
    : load_value(value), load_addr(addr), Instruction(block) {
    BaseTypePtr type_addr = load_addr->getBaseType();
    BaseTypePtr type_value = load_value->getBaseType();
    assert(type_addr->getAttrType() == type_value->getAttrType());
    assert(type_addr->IsPointer() && type_value->IsNotPtr());
    assert(type_addr->IsScalar() && type_value->IsScalar());
}

LoadInstPtr LoadInst::CreatePtr(VariablePtr value, BaseValuePtr addr, CfgNodePtr block) {
    return std::make_shared<LoadInst>(value, addr, block);
}

BaseValuePtr LoadInst::DoLoadValue(BaseValuePtr addr, CfgNodePtr block) {
    BaseTypePtr addr_type = addr->getBaseType();
    assert(addr_type->IsPointer() && addr_type->IsScalar() && (addr_type->IntType() || addr_type->FloatType()));
    VariablePtr value = Variable::CreatePtr(addr_type->IntType() ? type_int_L : type_float_L, nullptr);
    auto &&inst = CreatePtr(value, addr, block);
    value->SetParent(inst);
    addr->InsertUser(inst);
    block->InsertInstBack(inst);
    return value;
}

bool LoadInst::IsLoadInst() const { return true; }

std::string LoadInst::tollvmIR() {
    std::stringstream ss;
    ss << load_value->tollvmIR() << " = load " << load_value->getBaseType()->tollvmIR();
    ss << ", " << load_addr->getBaseType()->tollvmIR() << ' ' << load_addr->tollvmIR();
    ss << ", align 4";
    ss << "; Inst_" << GetInstIdx() << " from Block_" << parent->GetBlockIdx();
    return ss.str();
}

//===-----------------------------------------------------------===//
//                     GetElementPtrInst Implementation
//===-----------------------------------------------------------===//

GetElementPtrInst::GetElementPtrInst(VariablePtr _ptr, BaseTypePtr _type, BaseValuePtr _addr, BaseValueList _off,
                                     CfgNodePtr block)
    : target_ptr(_ptr), store_type(_type), base_addr(_addr), offset_list(_off), Instruction(block) {
    assert(target_ptr->getBaseType()->getAttrType() == store_type->getAttrType());
    assert(target_ptr->getBaseType()->getAttrType() == base_addr->getBaseType()->getAttrType());
    assert(store_type->IsNotPtr() && base_addr->getBaseType()->IsPointer());
    if (store_type->IsScalar()) {
        assert(offset_list.size() == 1);
    } else {
        ListTypePtr list1 = std::static_pointer_cast<ListType>(store_type);
        ListTypePtr list2 = std::static_pointer_cast<ListType>(base_addr->getBaseType());
        assert(list1->getArrSize() == list2->getArrSize());
        // assert(offset_list.size() == 2);
    }
}

GepInstPtr GetElementPtrInst::CreatePtr(VariablePtr _ptr, BaseTypePtr _type, BaseValuePtr _addr, BaseValueList _off,
                                        CfgNodePtr block) {
    return std::make_shared<GetElementPtrInst>(_ptr, _type, _addr, _off, block);
}

VariablePtr GetElementPtrInst::DoGetPointer(BaseTypePtr _type, BaseValuePtr _addr, BaseValueList _off,
                                            CfgNodePtr block) {
    // only have INT-array or FLOAT-array
    VariablePtr _ptr = Variable::CreatePtr(_type->IntType() ? type_int_ptr_L : type_float_ptr_L, nullptr);
    auto &&inst = CreatePtr(_ptr, _type, _addr, _off, block);
    _ptr->SetParent(inst);
    _addr->InsertUser(inst);
    std::for_each(_off.begin(), _off.end(), [&inst](const auto &offset) { offset->InsertUser(inst); });
    block->InsertInstBack(inst);
    return _ptr;
}

bool GetElementPtrInst::IsGepInst() const { return true; }

std::string GetElementPtrInst::tollvmIR() {
    std::stringstream ss;
    ss << target_ptr->tollvmIR() << " = getelementptr inbounds " << store_type->tollvmIR();
    ss << ", " << base_addr->getBaseType()->tollvmIR() << ' ' << base_addr->tollvmIR();
    for (auto &&offset : offset_list) {
        ss << ", " << offset->getBaseType()->tollvmIR() << ' ' << offset->tollvmIR();
    }
    ss << "; Inst_" << GetInstIdx() << " from Block_" << parent->GetBlockIdx();
    return ss.str();
}
