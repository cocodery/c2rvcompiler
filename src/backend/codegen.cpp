#include "codegen.hh"

CodeGen::CodeGen(const char *path, CompilationUnit &_comp_unit) : bbs(), comp_unit(_comp_unit) {
    Log("open file: %s", path);
    fs.open(path, std::ios::out);
    Assert(fs, "I/O Error");
}

void CodeGen::GenASM() {
    for (auto &&bb : bbs) {
        bb->GenASM(fs);
        fs.flush();
    }
}

void CodeGen::Reorder() {
    for (auto &&bb : bbs) {
        bb->Reorder();
    }
}

CodeGen::~CodeGen() { fs.close(); }