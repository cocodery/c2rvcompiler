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

void CompilationUnit::generatellvmIR(std::string &irfile) {
    std::ofstream llir;
    llir.open(irfile, std::ios_base::out);
    if (llir.is_open() == false) {
        assert(0);
    }
    for (auto [name, value] : glb_table.getNameValueMap()) {
        BaseTypePtr &&type = value->getBaseType();
        // there is no need to emit global-constant llvmIR
        if (type->ConstType() && type->ConstantType()) {
            continue;
        }
        llir << '@' << name << " = " << value->getBaseType();
        // as a '\b' to eat '*', on need to explict output '*' for global declaration
        llir.seekp(-1, std::ios::end);
        llir << " " << value << ", align 4" << endl;
    }
    llir << endl;
    for (auto [name, func_ptr] : func_talbe.getFunctionTable()) {
        llir << func_ptr << endl;
    }
}