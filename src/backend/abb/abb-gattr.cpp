#include "abb-gattr.hh"

void ABBGAttr::GenASM(std::fstream &fs) {
    fs << attr << std::endl;
}

void ABBGAttr::Reorder() {}