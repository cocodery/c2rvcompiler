#include "function.hh"

Function::Function(ScalarTypePtr _type, std::string &_name, ParamList &_list, BlockPtr _block)
    : ret_type(_type), func_name(_name), param_list(_list), block(_block) {
}

ScalarTypePtr Function::getReturnType() {
    return this->ret_type;
}

ParamList &Function::getParamList() {
    return this->param_list;
}

FunctionPtr Function::CreatePtr(ScalarTypePtr _type, std::string &_name, ParamList &_list, BlockPtr _block) {
    return std::make_shared<Function>(_type, _name, _list, _block);
}

std::string Function::toString() {
    std::stringstream ss;

    ss << ret_type->toString() << ' ' << func_name;
    ss << "( ";
    for (auto &&param : param_list) {
        ss << param.first << ": " << param.second->toString() << ' ';
    }
    ss << ')';

    return ss.str();
}

std::string Function::tollvmIR() {
    std::stringstream ss;

    ss << "define " << ret_type->tollvmIR() << " @" << func_name;
    ss << "(";

    size_t fparam_size = param_list.size();
    if (fparam_size > 0) {
        size_t idx = 0;
        ss << param_list[idx].second->getBaseType()->tollvmIR() << ' ' << param_list[idx].second->tollvmIR();
        for (idx = 1; idx < fparam_size; ++idx) {
            ss << ", " << param_list[idx].second->getBaseType()->tollvmIR() << ' ' << param_list[idx].second->tollvmIR();
        }
    }
    
    ss << ")" << " {" << endl;
    ss << block->tollvmIR() << endl;
    ss << "}" << endl;

    return ss.str();
}

std::ostream &operator<<(std::ostream &os, FunctionPtr func) {
    os << func->tollvmIR();
    return os;
}
