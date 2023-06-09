#pragma once

#include <fstream>

class ASMBasicBlock {
   public:
    virtual void GenASM(std::fstream &fs) = 0;
    virtual void Reorder() = 0;
};