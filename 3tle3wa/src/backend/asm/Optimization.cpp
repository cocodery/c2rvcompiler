#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"
#include "3tle3wa/backend/asm/AsmProgress.hh"
#include "3tle3wa/backend/asm/AsmSchedItem.hh"
#include "3tle3wa/backend/rl/InternalTranslation.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLSchedItem.hh"
#include "3tle3wa/backend/scheduler/FIFO.hh"
#include "3tle3wa/backend/scheduler/SchedMachine.hh"
#include "3tle3wa/utils/logs.hh"

void AsmProgress::DoOptimization() {
    for (auto &&abb : ablks_) {
        abb->Peepholes();
    }

    // after this scheduler
    // other optimizition should operator on view not on raw
    DoASchedule();
}

void AsmBasicBlock::Peepholes() {}

void AsmProgress::DoASchedule() {
    for (auto &&abb : ablks_) {
        abb->ArchSchedule();
    }
}

void AsmBasicBlock::ArchSchedule() {
    SchedMachine mach(std::make_unique<SchedFIFO>());

    for (auto &&inst : insts_) {
        auto item = std::make_unique<AsmSchedItem>();
        item->inst = inst.get();

        mach.Push(std::move(item));
    }

    mach.Sched();

    auto fifo = dynamic_cast<SchedFIFO *>(mach.Policy());

    for (auto &&item : fifo->View()) {
        auto asmitem = dynamic_cast<AsmSchedItem *>(item);
        insts_view_.push_back(asmitem->inst);
    }
}