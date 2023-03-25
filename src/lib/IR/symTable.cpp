#include "symTable.hh"

SymbolTable::SymbolTable(SymbolTable *_p) 
    : parent(_p), sym_table(NameValueMap()) {

}

NameValueMap &SymbolTable::getNameValueMap() {
    return this->sym_table;
}

SymbolTable *SymbolTable::getParentTable() {
    return this->parent;
}

void SymbolTable::insertSymbol(std::string &name, BaseValuePtr value) {
    sym_table.insert({name, value});
}