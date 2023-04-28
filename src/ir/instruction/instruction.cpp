#include "instruction.hh"

//===-----------------------------------------------------------===//
//                     Instruction Implementation
//===-----------------------------------------------------------===//

size_t Instruction::inst_idx = 1;

Instruction::Instruction(CfgNodePtr _parent) : idx(inst_idx++), parent(_parent) {}

const size_t Instruction::GetInstIdx() const { return idx; }
const CfgNodePtr Instruction::GetParent() const { return parent; }

bool Instruction::IsAllocaInst() const { return false; }
bool Instruction::IsLoadInst() const { return false; }
bool Instruction::IsStoreInst() const { return false; }
bool Instruction::IsGepInst() const { return false; }

bool Instruction::IsPhiInst() const { return false; }

//===-----------------------------------------------------------===//
//                     UnaryInstruction Implementation
//===-----------------------------------------------------------===//

UnaryInstruction::UnaryInstruction(VariablePtr _res, BaseValuePtr _opr, CfgNodePtr node)
    : result(_res), oprand(_opr), Instruction(node) {}

//===-----------------------------------------------------------===//
//                     BinaryInstruction Implementation
//===-----------------------------------------------------------===//

BinaryInstruction::BinaryInstruction(VariablePtr _res, OpCode _op, BaseValuePtr _lhs, BaseValuePtr _rhs,
                                     CfgNodePtr node)
    : result(_res), op(_op), lhs(_lhs), rhs(_rhs), Instruction(node) {}
