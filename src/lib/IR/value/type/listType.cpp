#include "listType.hh"

ListType::ListType(TypeID _tid, ArrDims &_dims, bool _omit) 
    : BaseType(_tid), dims(_dims), omit(_omit) {

}

std::string ListType::toString() {
    return "";
}

std::string ListType::tollvmIR() {
    return "";
}