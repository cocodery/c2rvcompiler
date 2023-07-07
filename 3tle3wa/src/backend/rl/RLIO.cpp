#include <cinttypes>

#include "3tle3wa/backend/InternalTranslation.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLGen.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLStackInfo.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"

extern const char *gpr[];

void RLGen::formatString(FILE *fp) {
    for (auto &&task : translation_tasks_) {
        fprintf(fp, "%s", task->CString());
    }
}

void InternalTranslation::formatString(FILE *fp) {
    // avoid format
    fprintf(fp, "%s", rlps_->CString());
}

void RLProgress::formatString(FILE *fp) {
    fprintf(fp, "\t# ===== progrem begin =====\n");
    fprintf(fp, "%s:\n", label_);
    for (auto &&bb : rlbbs_) {
        fprintf(fp, "%s", bb->CString());
    }
    fprintf(fp, "\t# =====  stack infos  =====\n");
    fprintf(fp, "%s", planner_->CString());
    fprintf(fp, "\t# =====  progrem end  =====\n\n");
}

void RLBasicBlock::formatString(FILE *fp) {
    fprintf(fp, ".b%" PRIu64 ":\n", lbidx_);

    fprintf(fp, "\t# LDef:");
    for (auto &&def : live_def_) {
        fprintf(fp, " %" PRIu64, def);
    }
    fputc('\n', fp);

    fprintf(fp, "\t# LUse:");
    for (auto &&use : live_use_) {
        fprintf(fp, " %" PRIu64, use);
    }
    fputc('\n', fp);

    fprintf(fp, "\t# LOut:");
    for (auto &&out : live_out_) {
        fprintf(fp, " %" PRIu64, out);
    }
    fputc('\n', fp);

    fprintf(fp, "\t# LIn :");
    for (auto &&in : live_in_) {
        fprintf(fp, " %" PRIu64, in);
    }
    fputc('\n', fp);

    for (auto &&uop : op_view_) {
        fprintf(fp, "%s", uop->CString());
    }
}

void VirtualRegister::formatString(FILE *fp) {
    fprintf(fp, "%%r%" PRIu64, vridx_);

    if (onstack_ and sinfo_ != nullptr) {
        fprintf(fp, "<%" PRIu64 ":%" PRId64 ">", sinfo_->GetSidx(), sinfo_->GetSLen());
        return;
    } else if (onstack_) {
        panic("ill formed");
    } else if (assigned_) {
        fprintf(fp, "<%s>", gpr[real_regidx_]);
    }
}

void RLPlanner::formatString(FILE *fp) {
    fprintf(fp, "stkidx : (    length,             offset) <- assume fp = .\n");
    uint64_t offs = 0;
    for (auto &&stk : stk_storage_) {
        offs -= stk->GetSLen();
        fprintf(fp, "%06" PRIu64 " : (%010" PRIu64 ", 0x%016" PRIx64 ") <- %" PRId64 "(fp)\n", stk->GetSidx(),
                stk->GetSLen(), stk->GetOff(), offs);
    }
    fputc('\n', fp);
}