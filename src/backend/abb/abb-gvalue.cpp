#include "abb-gvalue.hh"

void ABBGValue::GenASM(std::fstream &fs) {
    fs << "\t" ".global" "\t" << name << std::endl;
    fs << "\t" ".align" " " "4" << std::endl;
    fs << "\t" ".type" "\t" << name << ", " "@object" << std::endl;
    fs << "\t" ".size" "\t" << name << ", " << totalsiz * 4 << std::endl;
    fs << name << ":" << std::endl;
    if (isUninit) {
        fs << "\t" ".zero" "\t" << totalsiz * 4 << std::endl;
        return;
    }
    int cntz = 0;
    for (auto &&v: values) {
        if (v == 0) {
            cntz += 1;
            continue;
        }
        if (cntz) {
            fs << "\t" ".zero" "\t" << cntz * 4 << std::endl;
            cntz = 0;
        }
        fs << "\t" ".long" "\t" << v << std::endl;
    }
    if (cntz) {
        fs << "\t" ".zero" "\t" << cntz * 4 << std::endl;
    }
}

void ABBGValue::Reorder() {}