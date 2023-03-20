#include "IR.hh"

SymbolTable &CompilationUnit::getGlbTable() {
    return this->glb_table;
}

void CompilationUnit::insertSymbol(std::string &name, BaseValuePtr value) {
    glb_table.insertSymbol(name, value);
}

void CompilationUnit::printGlbTable() {
    for (auto [name, value] : glb_table.getNameValueMap()) {
        cout << name << ": " << value->toString() << endl;
    }
}