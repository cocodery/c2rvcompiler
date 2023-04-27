#pragma once

#include <list>

#include "function.hh"

using NormalFuncList = std::list<NormalFuncPtr>;
using LibFuncList = std::list<LibFuncPtr>;

class FunctionTable {
   private:
    NormalFuncList normalFuncTable;
    LibFuncList libraryFuncTable;

   public:
    FunctionTable();

    BaseFuncPtr getFunction(std::string &);

    void insertFunction(NormalFuncPtr);

    NormalFuncList &getNormalFuncTable();

    friend std::ostream &operator<<(std::ostream &, FunctionTable);
};
