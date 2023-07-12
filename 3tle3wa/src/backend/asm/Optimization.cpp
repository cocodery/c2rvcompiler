#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"
#include "3tle3wa/backend/asm/AsmProgress.hh"
#include "3tle3wa/utils/Logs.hh"

void AsmProgress::DoOptimization() {
    for (auto &&abb : ablks_) {
        abb->Peepholes();
    }
}

void AsmBasicBlock::Peepholes() {
    rmNeedless();

    combineInstruction();

    rmNeedless();
}

void AsmBasicBlock::rmNeedless() {
    //
    rmNeedlessStore();
}

void AsmBasicBlock::combineInstruction() {
    //
    combineAddLS();
}