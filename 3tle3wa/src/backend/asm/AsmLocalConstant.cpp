#include "3tle3wa/backend/asm/AsmLocalConstant.hh"

AsmLocalConstant::AsmLocalConstant(size_t idx, uint32_t values) : idx_(idx), values_(values) {
    data_len_ = 4;

    align_ = 4;
}

AsmLocalConstant::~AsmLocalConstant() {}

void AsmLocalConstant::formatString(FILE *fp) {
    fprintf(fp,
            // section
            "\t.section\t.srodata\n"

            // declare global
            "\t.global\t.LC%" PRIu64
            "\n"

            // alignment
            "\t.align\t%" PRIu64
            "\n"

            // type
            "\t.type\t.LC%" PRIu64
            ", @object\n"

            // data size
            "\t.size\t.LC%" PRIu64 ", %" PRIu64
            "\n"

            // label begin
            ".LC%" PRIu64
            ":\n"

            "\t.word\t%" PRIu32 "\n",
            idx_, align_, idx_, idx_, data_len_, idx_, values_);
}