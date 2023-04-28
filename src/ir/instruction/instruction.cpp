#include "instruction.hh"

size_t Instruction::inst_idx = 1;

Instruction::Instruction(CfgNodePtr _parent) : idx(inst_idx++), parent(_parent) {}

const size_t Instruction::GetInstIdx() const { return idx; }
const CfgNodePtr Instruction::GetParent() const { return parent; }

bool Instruction::IsAllocaInst() const { return false; }
bool Instruction::IsLoadInst() const { return false; }
bool Instruction::IsStoreInst() const { return false; }
bool Instruction::IsGepInst() const { return false; }

bool Instruction::IsPhiInst() const { return false; }
