#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "valueHeader.hh"
#include "symTable.hh"
#include "funcTable.hh"
#include "instHeader.hh"

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
    FunctionPtr getFunction(std::string &);
    void insertFunction(std::string &, FunctionPtr);
    void printFuncTalbe();

    void generatellvmIR(std::string &irile);
};
