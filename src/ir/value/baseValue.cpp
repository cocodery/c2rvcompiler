#include "baseValue.hh"

BaseValue::BaseValue(BaseTypePtr _type) : base_type(_type) {
    // attr_type != VOID
    assert(!base_type->VoidType());
}

BaseTypePtr BaseValue::getBaseType() { return this->base_type; }

bool BaseValue::IsOprand() { return (!base_type->VoidType() && base_type->IsNotPtr() && base_type->IsScalar()); }

const InstPtr BaseValue::GetParent() const { return InstPtr(nullptr); }

bool BaseValue::IsConstant() { return false; }

void BaseValue::fixValue(ATTR_TYPE) { return; }

InstList BaseValue::GetUserList() const { return use_list; }

void BaseValue::InsertUser(InstPtr inst) {
    if (IsConstant()) return;
    if (std::find(use_list.begin(), use_list.end(), inst) == use_list.end()) {
        use_list.push_back(inst);
    }
}

void BaseValue::RemoveUser(InstPtr inst) { use_list.remove(inst); }
