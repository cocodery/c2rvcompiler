#include "3tle3wa/backend/AsmGlobalValue.hh"

AsmGlobalValue::AsmGlobalValue(const std::string &name, size_t len, bool uninit, size_t reserve) {
    on_heap_ = false;

    data_len_ = len;

    uninit_ = uninit;

    align_ = 4;

    values_.reserve(reserve);

    FILE *fp = open_memstream(&name_, &name_len_);
    fprintf(fp, "%s", name.c_str());
    fflush(fp);
    fclose(fp);
}

AsmGlobalValue::~AsmGlobalValue() {
    if (name_) {
        free(name_);
        name_ = nullptr;
        name_len_ = 0;
    }
}

void AsmGlobalValue::Push(uint32_t value) { values_.push_back(value); }

bool AsmGlobalValue::OnHeap() const { return on_heap_; }

const char *AsmGlobalValue::Name() const { return name_; }

void AsmGlobalValue::formatString(FILE *fp) {
    if (uninit_) {
        fprintf(fp, "\t.bss\n");
    } else {
        fprintf(fp, "\t.data\n");
    }

    fprintf(fp,
            // declare global
            "\t.global\t%s\n"

            // alignment
            "\t.align\t%" PRIu64
            "\n"

            // type
            "\t.type\t%s, @object\n"

            // data size
            "\t.size\t%s, %" PRIu64
            "\n"

            // label begin
            "%s:\n",
            name_, align_, name_, name_, data_len_, name_);

    if (uninit_) {
        fprintf(fp, "\t.zero\t%" PRIu64 "\n", data_len_);
        return;
    }
    int cntz = 0;
    for (auto &&v : values_) {
        if (v == 0) {
            cntz += 1;
            continue;
        }

        if (cntz) {
            fprintf(fp, "\t.zero\t%" PRIu64 "\n", cntz * 4);
            cntz = 0;
        }
        fprintf(fp, "\t.word\t%" PRIu32 "\n", v);
    }
    if (cntz) {
        fprintf(fp, "\t.zero\t%" PRIu64 "\n", cntz * 4);
    }
}
