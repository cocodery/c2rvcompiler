#pragma once

#include "asmbasicblock.hh"

class ABBGAttr : public ASMBasicBlock {
   public:
    std::string attr;

    void GenASM(std::fstream &fs);
    void Reorder();
};