#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "valueHeader.hh"
#include "symTable.hh"
#include "funcTable.hh"

using std::cout;
using std::endl;

class CompilationUnit {
private:
    SymbolTable glb_table;
    FunctionTable func_talbe;
public:
    CompilationUnit() = default;
    ~CompilationUnit() = default;

    SymbolTable &getGlbTable();
    void insertSymbol(std::string &, BaseValuePtr);
    void printGlbTable();

    FunctionTable &getFuncTable();
    void insertFunction(std::string &, FunctionPtr);
    void printFuncTalbe();
};
