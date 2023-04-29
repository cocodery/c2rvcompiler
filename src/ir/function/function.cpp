#include "function.hh"

//===-----------------------------------------------------------===//
//                     NormalFunction Implementation
//===-----------------------------------------------------------===//

NormalFunction::NormalFunction(ScalarTypePtr _type, std::string &_name, ParamList &_list)
    : BaseFunction(_type, _name, _list) {}

CfgNodePtr NormalFunction::CreateEntry() {
    entry = CtrlFlowGraphNode::CreatePtr();
    return entry;
}

CfgNodePtr NormalFunction::CreateExit() {
    exit = CtrlFlowGraphNode::CreatePtr();
    return exit;
}

CfgNodePtr NormalFunction::CreateCfgNode() { return CtrlFlowGraphNode::CreatePtr(); }

CfgNodePtr NormalFunction::GetEntryNode() { return entry; }
CfgNodePtr NormalFunction::GetExitNode() { return exit; }

CfgNodeList NormalFunction::GetAllNodes() {
    CfgNodeList allNodes;
    std::queue<CfgNodePtr> nodeQueue;
    nodeQueue.push(entry);
    while (!nodeQueue.empty()) {
        auto &&front = nodeQueue.front();
        nodeQueue.pop();
        // if queue-front not in visitMap
        if (std::find(allNodes.begin(), allNodes.end(), front) == allNodes.end()) {
            allNodes.push_back(front);
            for (auto &&node : front->GetSuccessors()) {
                nodeQueue.push(node);
            }
        }
    }
    assert(nodeQueue.empty());
    return allNodes;
}

void NormalFunction::SetVarIdx(size_t _var_idx) { var_idx = _var_idx; }
size_t NormalFunction::GetVarIdx() { return var_idx; }

void NormalFunction::SetBlkIdx(size_t _blk_idx) { blk_idx = _blk_idx; }
size_t NormalFunction::GetBlkIdx() { return blk_idx; }

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

    ss << ") {" << endl;

    for (auto &&node : GetAllNodes()) {
        ss << node->tollvmIR() << endl;
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
