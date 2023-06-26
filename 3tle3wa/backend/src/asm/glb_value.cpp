#include "3tle3wa/backend/asm/glb_value.hh"

glb_value::glb_value(size_t glb_idx, size_t len, bool uninit, size_t reserve)
    : glb_idx_(glb_idx), len_(len), uninit_(uninit), values_(reserve) {
    values_.clear();
}

void glb_value::push(uint32_t value) { values_.push_back(value); }

void glb_value::gen_asm(std::fstream &fs) {
    auto label = gen_glb_val_label(glb_idx_);

    fs << "\t.global\t" << label << std::endl;
    fs << "\t.align\t" << 4 << std::endl;
    fs << "\t.type\t" << label << ", @object" << std::endl;
    fs << "\t.size\t" << label << ", " << len_ * 4 << std::endl;
    fs << label << ":" << std::endl;
    if (uninit_) {
        fs << "\t.zero\t" << len_ * 4 << std::endl;
        return;
    }
    int cntz = 0;
    for (auto &&v : values_) {
        if (v == 0) {
            cntz += 1;
            continue;
        }
        if (cntz) {
            fs << "\t.zero\t" << cntz * 4 << std::endl;
            cntz = 0;
        }
        fs << "\t.word\t" << v << std::endl;
    }
    if (cntz) fs << "\t.zero\t" << cntz * 4 << std::endl;
}