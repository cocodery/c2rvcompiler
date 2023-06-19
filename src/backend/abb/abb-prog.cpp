#include "abb-prog.hh"

#include <iomanip>

void ABBProg::GenASM(std::fstream &fs) {
    fs << "\t" ".text" << std::endl;
    fs << "\t" ".global" "\t" << name << std::endl;
    fs << "\t" ".type" "\t" << name << ", " "@function" << std::endl;
    fs << name << ":" << std::endl;
    for (auto &&abb: abbs) {
        fs << abb->tagname << ":" << std::endl;
        for (auto &&inst: abb->asminsts) {
            auto &&inststr = inst->toString();
            fs << "\t" << std::left << std::setw(40) << inststr << inst->Comment() << std::endl;
        }
    }
    fs << "\t" ".size" "\t" + name + ", .-" + name << std::endl;

    for (auto &&loc: locs) {
        loc->GenASM(fs);
    }
}

void ABBProg::Reorder() {
    for (auto &&abb: abbs) {
        abb->Reorder();
    }
}