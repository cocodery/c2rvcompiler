#include "3tle3wa/backend/asm/AsmProgress.hh"

#include <string>

#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/utils/Logs.hh"

AsmProgress::AsmProgress(std::string &name) {
    ret_ = nullptr;

    FILE *fp = open_memstream(&label_, &label_len_);
    fprintf(fp, "%s", name.c_str());
    fflush(fp);
    fclose(fp);
}

AsmProgress::~AsmProgress() {
    if (label_ != nullptr) {
        free(label_);
        label_ = nullptr;
        label_len_ = 0;
    }
}

void AsmProgress::Push(std::unique_ptr<AsmBasicBlock> &ablk, bool isRet) {
    if (isRet) {
        if (ret_ != nullptr) {
            panic("double ret block");
        }
        ret_ = ablk.get();
    }
    ablks_.push_back(std::move(ablk));
}

AsmBasicBlock *AsmProgress::CreateEntryBlock() {
    ablks_.push_front(std::make_unique<AsmBasicBlock>());
    return ablks_.front().get();
}

AsmBasicBlock *AsmProgress::ReturnBlock() { return ret_; }

const char *AsmProgress::Label() const {
    return label_;
}

void AsmProgress::formatString(FILE *fp) {
    fprintf(fp, "%s:\n", label_);
    for (auto &&blk: ablks_) {
        fprintf(fp, "%s", blk->CString());
    }
}