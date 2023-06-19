#pragma once

#include <list>
#include <memory>

#include "../spec/asm.hh"
#include "abb-gvalue.hh"
#include "asmbasicblock.hh"

class ABBProg : public ASMBasicBlock {
   public:
    class ABBlock {
       public:
        std::string tagname;
        std::list<std::shared_ptr<ASMInst>> asminsts;

        void Reorder();
    };

   public:
    std::string name;
    std::list<std::shared_ptr<ABBlock>> abbs;
    std::list<std::shared_ptr<ABBGValue>> locs;

    void GenASM(std::fstream &fs);
    void Reorder();
};