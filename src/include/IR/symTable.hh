#pragma once

#include <vector>
#include <unordered_map>

#include "valueHeader.hh"

using NameValue = std::pair<std::string, BaseValuePtr>;
using NameValueMap = std::unordered_map<std::string, BaseValuePtr>;

class SymbolTable {
private:
    SymbolTable *parent;
    NameValueMap sym_table;
public:
    SymbolTable(SymbolTable *_p = nullptr);

    NameValueMap &getNameValueMap();

    SymbolTable *getParentTable();

    void insertSymbol(std::string &, BaseValuePtr);
};

using SymTableList = std::vector<SymbolTable *>;