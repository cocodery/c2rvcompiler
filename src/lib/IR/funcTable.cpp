#include "funcTable.hh"

NameFuncMap &FunctionTable::getFunctionTable() {
    return this->func_table;
}

void FunctionTable::insertFunction(std::string &_name, FunctionPtr func_ptr) {
    func_table.push_back({_name, func_ptr});
}