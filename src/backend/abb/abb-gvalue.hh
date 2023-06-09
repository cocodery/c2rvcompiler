#pragma once

#include "asmbasicblock.hh"

#include <vector>

class ABBGValue : public ASMBasicBlock {
   public:
    std::string name;
    size_t totalsiz;
    bool isUninit;
    std::vector<uint32_t> values;

    void GenASM(std::fstream &fs);
    void Reorder();
};