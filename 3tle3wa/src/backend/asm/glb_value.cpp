#include "3tle3wa/backend/asm/glb_value.hh"

glb_value::glb_value(const std::string &name, size_t len, bool uninit, size_t reserve)
    : label_(name), len_(len), values_(reserve), uninit_(uninit) {
    // if (len >= 2048) {
    //     onheap_ = true;
    // }
    values_.clear();
}

void glb_value::push(uint32_t value) { values_.push_back(value); }

bool glb_value::onheap() { return onheap_; }

std::string &glb_value::name() { return label_; }

void glb_value::gen_asm(std::fstream &fs) {
    if (uninit_) {
        fs << "\t.bss" << std::endl;
    } else {
        fs << "\t.data" << std::endl;
    }
    fs << "\t.global\t" << label_ << std::endl;
    fs << "\t.align\t" << 4 << std::endl;
    fs << "\t.type\t" << label_ << ", @object" << std::endl;
    fs << "\t.size\t" << label_ << ", " << len_ * 4 << std::endl;
    fs << label_ << ":" << std::endl;
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