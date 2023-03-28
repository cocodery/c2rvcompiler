#include "function.hh"

Function::Function(ScalarTypePtr _type, std::string &_name, ParamList &_list, BlockPtr _block)
    : ret_type(_type), func_name(_name), param_list(_list), block(_block) {

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
    ss << "(" << ")" << " {" << endl;
    ss << block->tollvmIR() << endl;
    ss << "}" << endl;

    return ss.str();
}

std::ostream &operator<<(std::ostream &os, FunctionPtr func) {
    os << func->tollvmIR();
    return os;
}
