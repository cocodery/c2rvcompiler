#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmProgress.hh"

void AsmProgress::ArchSchedule() {
    for (auto &&abb : ablks_) {
        abb->ArchSchedule();
    }
}