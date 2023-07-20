#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"
#include "3tle3wa/backend/asm/AsmProgress.hh"
#include "3tle3wa/utils/logs.hh"

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
    rmNeedlessLS();
}

void AsmBasicBlock::combineInstruction() {
    //
    combineAddLS();
}