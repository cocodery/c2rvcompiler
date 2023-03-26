#include "function.hh"

Function::Function(ScalarTypePtr _type, std::string &_name, ParamList &_list)
    : ret_type(_type), func_name(_name), param_list(_list), block(nullptr) {

}

ParamList &Function::getParamList() {
    return this->param_list;
}

void Function::setBlock(BlockPtr block) {
    this->block = block;
}

std::shared_ptr<Function> Function::CreatePtr(ScalarTypePtr _type, std::string &_name, ParamList &_list) {
    return std::make_shared<Function>(_type, _name, _list);
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