#include "instruction.hh"

Instruction::Instruction(CfgNodePtr _parent) : parent(_parent) {}
const CfgNodePtr Instruction::GetParent() const { return parent; }

bool Instruction::IsAllocaInst() const { return false; }
bool Instruction::IsLoadInst() const { return false; }
bool Instruction::IsStoreInst() const { return false; }
bool Instruction::IsGepInst() const { return false; }

bool Instruction::IsPhiInst() const { return false; }
