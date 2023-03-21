#pragma once

#include <vector>

#include "function.hh"

using NameFunc    = std::pair<std::string, FunctionPtr>;
using NameFuncMap = std::vector<NameFunc>;

class FunctionTable {
private:
    NameFuncMap func_table;
public:
    FunctionTable() = default;

    NameFuncMap &getFunctionTable();

    void insertFunction(std::string &, FunctionPtr);
};