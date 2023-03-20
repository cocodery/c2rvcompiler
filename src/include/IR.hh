#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "valueHeader.hh"
#include "symTable.hh"

using std::cout;
using std::endl;

class CompilationUnit {
private:
    SymbolTable glb_table;
public:
    CompilationUnit() = default;
    ~CompilationUnit() = default;

    SymbolTable &getGlbTable();
    void insertSymbol(std::string &, BaseValuePtr);
    void printGlbTable();
};
