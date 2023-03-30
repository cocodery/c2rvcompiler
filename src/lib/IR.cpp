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
    for (auto [name, glb_value] : glb_table.getNameValueMap()) {
        BaseTypePtr &&type = glb_value->getBaseType();
        // // there is no need to emit global-constant llvmIR
        if (type->ConstType() && type->ConstantType()) {
            llir << "; @" << name << " = " << type << ' ' << glb_value << ", align 4" << endl; 
            continue;
        }
        llir << glb_value->tollvmIR() << " = ";
        BaseValuePtr init_value = std::static_pointer_cast<GlobalValue>(glb_value)->getInitValue();
        llir << init_value->getBaseType() << " " << init_value << ", align 4" << endl;
    }
    llir << endl;
    for (auto [name, func_ptr] : func_talbe.getFunctionTable()) {
        llir << func_ptr << endl;
    }
}