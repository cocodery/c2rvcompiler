#pragma once

#include <fstream>
#include <list>
#include <memory>

#include "../ir/IR.hh"
#include "abb/asmbasicblock.hh"
#include "spec/asm.hh"

class CodeGen {
    std::list<std::shared_ptr<ASMBasicBlock>> bbs;
    std::fstream fs;
    CompilationUnit &comp_unit;

   public:
    CodeGen(const char *path, CompilationUnit &_comp_unit);
    ~CodeGen();

    void GenABB();
    void Reorder();
    void GenASM();
};