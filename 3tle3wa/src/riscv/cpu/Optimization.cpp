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
#include "3tle3wa/backend/scheduler/SiFiveU74.hh"
#include "3tle3wa/utils/logs.hh"

void AsmProgress::DoOptimization() {
    for (auto &&abb : ablks_) {
        abb->Peepholes();
    }

    DoASchedule();
}

void AsmBasicBlock::Peepholes() {}

void AsmProgress::DoASchedule() {
    for (auto &&abb : ablks_) {
        abb->ArchSchedule();
    }
}

void AsmBasicBlock::ArchSchedule() {
    do /* FIFO */ {
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
    } while (0);

    return;

    do /* schedule */ {
        SchedMachine mach(std::make_unique<SchedSiFiveU74>());

        for (auto &&inst : insts_) {
            auto item = std::make_unique<AsmSchedItem>();
            item->inst = inst.get();
            item->type = inst->GetSchedType();

            item->memop = (item->type == SCHED_TYPE::LOAD) or (item->type == SCHED_TYPE::STORE);

            if (auto res = inst->GetResult(); res != 0) {
                item->writes.push_back(res);
            }

            if (auto opds = inst->GetOperands(); not opds.empty()) {
                for (auto &&opd : opds) {
                    item->reads.push_back(opd);
                }
            }

            mach.Push(std::move(item));
        }

        insts_view_.clear();

        mach.Sched();

        auto sifiveu74 = dynamic_cast<SchedSiFiveU74 *>(mach.Policy());

        for (auto &&item : sifiveu74->View()) {
            auto asmitem = dynamic_cast<AsmSchedItem *>(item);
            insts_view_.push_back(asmitem->inst);
        }
    } while (0);
}