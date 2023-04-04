#pragma once

#include <vector>

#include "function.hh"

using FuncMap = std::vector<BaseFuncPtr>;

class FunctionTable {
private:
    FuncMap libfunc_table;
    FuncMap usrfunc_table;
public:
    FunctionTable();

    BaseFuncPtr getFunction(std::string &);

    void insertFunction(BaseFuncPtr);

    friend std::ostream &operator<<(std::ostream &, FunctionTable);
};
