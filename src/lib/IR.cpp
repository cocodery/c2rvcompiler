#include "IR.hh"

SymbolTable &CompilationUnit::getGlbTable() { return this->glb_table; }

void CompilationUnit::insertSymbol(std::string &name, BaseValuePtr value) { glb_table.insertSymbol(name, value); }

FunctionTable &CompilationUnit::getFuncTable() { return this->func_talbe; }

BaseFuncPtr CompilationUnit::getFunction(std::string &name) { return this->func_talbe.getFunction(name); }

void CompilationUnit::insertFunction(NormalFuncPtr func_ptr) { func_talbe.insertFunction(func_ptr); }

void CompilationUnit::generatellvmIR(std::string &irfile) {
    std::ofstream llir;
    llir.open(irfile, std::ios_base::out);
    if (llir.is_open() == false) {
        assert(0);
    }
    llir << "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"" << endl;
    for (auto [name, glb_value] : glb_table.getNameValueMap()) {
        BaseTypePtr &&type = glb_value->getBaseType();
        // // there is no need to emit global-constant llvmIR
        if (type->IsImMutable() && type->IsScalar()) {
            llir << "; @" << name << " = " << type->tollvmIR() << ' ' << glb_value->tollvmIR() << ", align 4";
        } else {
            llir << glb_value->tollvmIR() << " = ";
            if (type->IsImMutable()) {
                llir << "constant ";
            } else if (type->IsGlobal()) {
                llir << "global ";
            }
            BaseValuePtr init_value = std::static_pointer_cast<GlobalValue>(glb_value)->getInitValue();
            llir << init_value->getBaseType()->tollvmIR() << " " << init_value->tollvmIR() << ", align 4";
        }
        llir << "; " << name << ' ' << glb_value->getBaseType()->tollvmIR() << endl;
    }
    llir << endl << this->func_talbe << endl;
}