#include "3tle3wa/backend/asm/AsmGen.hh"

#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmGlobalValue.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"
#include "3tle3wa/backend/asm/AsmLocalConstant.hh"
#include "3tle3wa/backend/asm/AsmProgress.hh"

void AsmGen::formatString(FILE *fp) {
    fprintf(fp,
            // no pic
            "\t.option nopic\n"

            // arch info
            "\t.attribute arch, \"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0\"\n"

            // aligned access
            "\t.attribute unaligned_access, 0\n"

            // stack aligned
            "\t.attribute stack_align, 16\n");

    for (auto &&gv : gv_storage_) {
        fprintf(fp, "%s", gv->CString());
    }

    for (auto &&pg : pg_storage_) {
        fprintf(fp, "%s", pg->CString());
    }

    for (auto &&lc : lc_storage_) {
        fprintf(fp, "%s", lc->CString());
    }

    // compiler tag
    fprintf(fp, "\t.ident\t\"c2rv: 0.2\"\n");
}

void AsmGen::PushAsmGlobalValue(std::unique_ptr<AsmGlobalValue> &agv) { gv_storage_.push_back(std::move(agv)); }

void AsmGen::PushAsmLocalConstant(std::unique_ptr<AsmLocalConstant> &alc) { lc_storage_.push_back(std::move(alc)); }

void AsmGen::PushAsmProgress(std::unique_ptr<AsmProgress> &ap) { pg_storage_.push_back(std::move(ap)); }
