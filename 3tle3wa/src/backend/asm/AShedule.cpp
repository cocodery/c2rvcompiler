#include "3tle3wa/backend/asm/AsmProgress.hh"
#include "3tle3wa/backend/asm/AsmBasicBlock.hh"

void AsmProgress::ArchSchedule() {
    for (auto &&abb : ablks_) {
        abb->ArchSchedule();
    }
}