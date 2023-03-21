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

FunctionTable &CompilationUnit::getFuncTable() {
    return this->func_talbe;
}

void CompilationUnit::insertFunction(std::string &_name, FunctionPtr func_ptr) {
    func_talbe.insertFunction(_name, func_ptr);
}

void CompilationUnit::printFuncTalbe() {
    for (auto [name, func_ptr] : func_talbe.getFunctionTable()) {
        cout << "function -> " << func_ptr->toString() << endl;
    }
}