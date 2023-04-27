#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "funcTable.hh"
#include "instHeader.hh"
#include "symTable.hh"
#include "valueHeader.hh"

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
    void InsertSymbol(std::string &, BaseValuePtr);

    NormalFuncList &GetNormalFuncTable();
    BaseFuncPtr GetFunction(std::string &);
    void InsertFunction(NormalFuncPtr);

    void generatellvmIR(std::string &irile);
};
