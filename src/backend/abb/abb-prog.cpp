#include "abb-prog.hh"

void ABBProg::GenASM(std::fstream &fs) {
    fs << "\t" ".text" << std::endl;
    fs << "\t" ".global" "\t" << name << std::endl;
    fs << "\t" ".type" "\t" << name << ", " "@function" << std::endl;
    fs << name << ":" << std::endl;
    fs << "\t" << ".cfi_startproc" << std::endl;
    for (auto &&abb: abbs) {
        fs << abb->tagname << ":" << std::endl;
        for (auto &&inst: abb->asminsts) {
            fs << "\t" << inst->toString() << "\t" << inst->Comment() << std::endl;
        }
    }
    fs << "\t" << ".cfi_endproc" << std::endl;
}

void ABBProg::Reorder() {
    for (auto &&abb: abbs) {
        abb->Reorder();
    }
}