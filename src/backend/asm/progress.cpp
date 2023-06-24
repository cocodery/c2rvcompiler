#include "progress.hh"

#include "../utils.hh"
#include "riscv/asm.hh"

pblock::pblock(size_t lbidx, size_t reserve) : lbidx_(lbidx), insts_(reserve) { insts_.clear(); }

void pblock::gen_asm(std::fstream &fs) {
    fs << gen_pblk_label(lbidx_) << ":" << std::endl;
    for (auto &&inst : insts_) {
        fs << "\t" << inst->to_string();
        if (inst->comment().length()) {
            fs << "\t" << inst->comment();
        }
        fs << std::endl;
    }
}

void pblock::push(asm_inst *inst) { insts_.push_back(std::unique_ptr<asm_inst>(inst)); }

std::list<std::unique_ptr<asm_inst>> &pblock::ilst() { return insts_; }

void progress::gen_asm(std::fstream &fs) {
    fs << "\t.text" << std::endl;
    fs << "\t.align\t" << 1 << std::endl;
    fs << "\t.global\t" << label_ << std::endl;
    fs << "\t.type\t" << label_ << ", @function" << std::endl;
    fs << label_ << ":" << std::endl;
    for (auto &&pblk : pblks_) {
        pblk->gen_asm(fs);
    }
    fs << "\t.size\t" << label_ << ", .-" << label_ << std::endl;
}

progress::progress(std::string label, size_t reserve) : label_(label), pblks_(reserve) { pblks_.clear(); }

void progress::push(std::unique_ptr<pblock> &pblk) { pblks_.push_back(std::move(pblk)); }

std::unique_ptr<pblock> &progress::front() { return pblks_.front(); }

std::unique_ptr<pblock> &progress::back() {
    for (auto &&pblk: pblks_) {
        auto &&last = pblk->ilst().back();
        if (auto p = dynamic_cast<rv_ret *>(last.get()); p != nullptr) {
            return pblk;
        }
    }
    return pblks_.back();
}