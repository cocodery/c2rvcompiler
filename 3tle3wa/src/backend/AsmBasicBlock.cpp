#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"

AsmBasicBlock::AsmBasicBlock(size_t lbidx, AsmProgress *father) : lbidx_(lbidx), father_(father) {}

void AsmBasicBlock::Push(AsmInstruction *inst) {
    insts_.push_back(std::unique_ptr<AsmInstruction>(inst));
}