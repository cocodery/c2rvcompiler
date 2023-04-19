#include "function.hh"

//===-----------------------------------------------------------===//
//                     BaseFunction Implementation
//===-----------------------------------------------------------===//

BaseFunction::BaseFunction(ScalarTypePtr _type, std::string &_name, ParamList &_list)
    : ret_type(_type), func_name(_name), param_list(_list) {}

ScalarTypePtr BaseFunction::getReturnType() { return this->ret_type; }

std::string &BaseFunction::getFuncName() { return this->func_name; }

ParamList &BaseFunction::getParamList() { return this->param_list; }

//===-----------------------------------------------------------===//
//                     NormalFunction Implementation
//===-----------------------------------------------------------===//

NormalFunction::NormalFunction(ScalarTypePtr _type, std::string &_name, ParamList &_list)
    : BaseFunction(_type, _name, _list) {}

BlockPtr NormalFunction::createBB(bool insertable) {
    BlockPtr block = BasicBlock::CreatePtr();
    if (insertable) {
        this->block_list.push_back(block);
    }
    return block;
}

NormalFuncPtr NormalFunction::CreatePtr(ScalarTypePtr _type, std::string &_name, ParamList &_list) {
    return std::make_shared<NormalFunction>(_type, _name, _list);
}

std::string NormalFunction::tollvmIR() {
    std::stringstream ss;

    ss << "define " << ret_type->tollvmIR() << " @" << func_name << "(";
    size_t param_size = param_list.size();
    if (param_size > 0) {
        size_t idx = 0;
        ss << param_list[idx]->getBaseType()->tollvmIR() << " " << param_list[idx]->tollvmIR();
        for (idx = 1; idx < param_size; ++idx) {
            ss << ", " << param_list[idx]->getBaseType()->tollvmIR() << " " << param_list[idx]->tollvmIR();
        }
    }

    ss << ")"
       << " {" << endl;
    for (auto &&block : block_list) {
        ss << block->tollvmIR() << endl;
    }
    ss << "}" << endl;

    return ss.str();
}

//===-----------------------------------------------------------===//
//                     LibraryFunction Implementation
//===-----------------------------------------------------------===//

LibraryFunction::LibraryFunction(ScalarTypePtr _type, std::string &_name, ParamList &_list)
    : BaseFunction(_type, _name, _list) {}

LibFuncPtr LibraryFunction::CreatePtr(ScalarTypePtr _type, std::string _name, ParamList &_list) {
    return std::make_shared<LibraryFunction>(_type, _name, _list);
}

std::string LibraryFunction::tollvmIR() {
    std::stringstream ss;

    ss << "declare " << ret_type->tollvmIR() << " @" << func_name << "(";
    size_t param_size = param_list.size();
    if (param_size > 0) {
        size_t idx = 0;
        ss << param_list[idx]->getBaseType()->tollvmIR();
        for (idx = 1; idx < param_size; ++idx) {
            ss << ", " << param_list[idx]->getBaseType()->tollvmIR();
        }
    }
    ss << ")";

    return ss.str();
}

std::ostream &operator<<(std::ostream &os, BaseFuncPtr func) {
    os << func->tollvmIR();
    return os;
}
