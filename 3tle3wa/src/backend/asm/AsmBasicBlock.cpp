#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"
#include "3tle3wa/backend/asm/AsmProgress.hh"

#include <cinttypes>

AsmBasicBlock::AsmBasicBlock(size_t lbidx, AsmProgress *father) : lbidx_(lbidx), father_(father) {}

void AsmBasicBlock::Push(AsmInstruction *inst) {
    insts_.push_back(std::unique_ptr<AsmInstruction>(inst));
}

void AsmBasicBlock::formatString(FILE *fp) {
    fprintf(fp, ".L.%s.b%" PRIu64 ":\n", father_->Label(), lbidx_);
    for (auto &&inst: insts_) {
        fprintf(fp, "%s", inst->CString());
    }
}