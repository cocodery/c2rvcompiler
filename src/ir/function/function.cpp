#include "function.hh"

//===-----------------------------------------------------------===//
//                     NormalFunction Implementation
//===-----------------------------------------------------------===//

NormalFunction::NormalFunction(ScalarTypePtr _type, std::string &_name, ParamList &_list)
    : BaseFunction(_type, _name, _list) {}

CfgNodePtr NormalFunction::CreateEntry() {
    entry = CtrlFlowGraphNode::CreatePtr(ENTRY | NORMAL);
    return entry;
}

CfgNodePtr NormalFunction::CreateExit() {
    exit = CtrlFlowGraphNode::CreatePtr(EXIT | NORMAL);
    return exit;
}

CfgNodePtr NormalFunction::CreateCfgNode(BlockAttr _attr) { return CtrlFlowGraphNode::CreatePtr(_attr); }

CfgNodePtr NormalFunction::GetEntryNode() { return entry; }
CfgNodePtr NormalFunction::GetExitNode() { return exit; }

void NormalFunction::SetEntryNode(CfgNodePtr _entry) { entry = _entry; }
void NormalFunction::SetExitNode(CfgNodePtr _exit) { exit = _exit; }

CfgNodeList NormalFunction::TopoSortFromEntry() {
    std::map<CfgNodePtr, bool> visit;
    CfgNodeList preorder_node = CfgNodeList();

    auto PredAllVisited = [&visit](CfgNodePtr succ) {
        for (auto &&pred : succ->GetPredcessors()) {
            if (pred->FindBlkAttr(CONTINUE) || pred->FindBlkAttr(LOOPEND)) continue;
            if (pred->GetDominatorSet().size() == 0) continue;
            if (visit[pred] == false) return false;
        }
        return true;
    };

    std::function<void(CfgNodePtr)> DepthFirstSearch = [&](CfgNodePtr node) {
        visit[node] = true;
        preorder_node.push_back(node);
        for (auto &&succ : node->GetSuccessors()) {
            if (!visit[succ] && PredAllVisited(succ)) {
                DepthFirstSearch(succ);
            }
        }
    };
    DepthFirstSearch(entry);
    return preorder_node;
}

CfgNodeList NormalFunction::TopoSortFromExit() {
    std::map<CfgNodePtr, bool> visit;
    CfgNodeList postorder_node = CfgNodeList();

    auto SuccAllVisited = [&visit](CfgNodePtr pred) {
        if (pred->GetBlockAttr() == LOOPBEGIN) return true;
        for (auto &&succ : pred->GetSuccessors()) {
            if (visit[succ] == false) return false;
        }
        return true;
    };

    std::function<void(CfgNodePtr)> DepthFirstSearch = [&](CfgNodePtr node) {
        visit[node] = true;
        postorder_node.push_back(node);
        for (auto &&pred : node->GetPredcessors()) {
            if (!visit[pred]) {
                DepthFirstSearch(pred);
            }
        }
    };
    DepthFirstSearch(exit);
    return postorder_node;
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

    for (auto &&node : TopoSortFromEntry()) {
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
