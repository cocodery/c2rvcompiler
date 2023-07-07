#include "3tle3wa/ir/function/function.hh"
#include "3tle3wa/utils/Logs.hh"

#include <cstddef>
#include <memory>
#include <string>

//===-----------------------------------------------------------===//
//                     NormalFunction Implementation
//===-----------------------------------------------------------===//

NormalFunction::NormalFunction(ScalarTypePtr _type, std::string &_name, ParamList &_list)
    : BaseFunction(_type, _name, _list), tail_call(false) {}

CfgNodePtr NormalFunction::CreateEntry() {
    entry = CtrlFlowGraphNode::CreatePtr(ENTRY | NORMAL);
    return entry;
}

bool NormalFunction::IsLibFunction() const { return false; }

CfgNodePtr NormalFunction::CreateExit() {
    exit = CtrlFlowGraphNode::CreatePtr(EXIT | NORMAL);
    return exit;
}

CfgNodePtr NormalFunction::CreateCfgNode(BlockAttr _attr) { return CtrlFlowGraphNode::CreatePtr(_attr); }

CfgNodePtr NormalFunction::GetEntryNode() { return entry; }
CfgNodePtr NormalFunction::GetExitNode() { return exit; }

void NormalFunction::SetEntryNode(CfgNodePtr _entry) { entry = _entry; }
void NormalFunction::SetExitNode(CfgNodePtr _exit) { exit = _exit; }

void NormalFunction::SetTailCall(bool _tail_call) { tail_call = _tail_call; }
bool NormalFunction::GetTailCall() const { return tail_call; }

CfgNodeList NormalFunction::TopoSortFromEntry() {
    std::unordered_map<CfgNodePtr, bool> visit;
    CfgNodeList preorder_node = CfgNodeList();

    CRVC_UNUSE auto PredAllVisited = [&visit](CfgNodePtr succ) {
        if (succ->FindBlkAttr(LOOPBEGIN)) return true;
        for (auto &&pred : succ->GetPredcessors()) {
            if (pred->GetDominatorSet().size() == 0) continue;
            if (visit[pred] == false) return false;
        }
        return true;
    };

    std::function<void(CfgNodePtr)> DepthFirstSearch = [&](CfgNodePtr node) {
        visit[node] = true;
        preorder_node.push_back(node);
        for (auto &&succ : node->GetSuccessors()) {
            if (!visit[succ]) {
                DepthFirstSearch(succ);
            }
        }
    };
    DepthFirstSearch(entry);
    return preorder_node;
}

CfgNodeList NormalFunction::TopoSortFromExit() {
    std::unordered_map<CfgNodePtr, bool> visit;
    CfgNodeList postorder_node = CfgNodeList();

    CRVC_UNUSE auto SuccAllVisited = [&visit](CfgNodePtr pred) {
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
        ss << param_list[idx]->GetBaseType()->tollvmIR() << " " << param_list[idx]->tollvmIR();
        for (idx = 1; idx < param_size; ++idx) {
            ss << ", " << param_list[idx]->GetBaseType()->tollvmIR() << " " << param_list[idx]->tollvmIR();
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
    : BaseFunction(_type, _name, _list, true) {
    assert(side_effect == true && recursive == false);
}

bool LibraryFunction::IsLibFunction() const { return true; }

std::string LibraryFunction::tollvmIR() {
    std::stringstream ss;

    ss << "declare " << ret_type->tollvmIR() << " @" << func_name << "(";
    size_t param_size = param_list.size();
    if (param_size > 0) {
        size_t idx = 0;
        ss << param_list[idx]->GetBaseType()->tollvmIR();
        for (idx = 1; idx < param_size; ++idx) {
            ss << ", " << param_list[idx]->GetBaseType()->tollvmIR();
        }
    }
    ss << ")";

    return ss.str();
}

//===-----------------------------------------------------------===//
//                     SYSYLibFunction Implementation
//===-----------------------------------------------------------===//

SYSYLibFunction::SYSYLibFunction(ScalarTypePtr _type, std::string &_name, ParamList &_list)
    : LibraryFunction(_type, _name, _list) {}

bool SYSYLibFunction::IsSYSYLibFunction() const { return true; }

SYSYLibFuncPtr SYSYLibFunction::CreatePtr(ScalarTypePtr _type, std::string _name, ParamList &_list) {
    return std::make_shared<SYSYLibFunction>(_type, _name, _list);
}

//===-----------------------------------------------------------===//
//                     LLVMLibFunction Implementation
//===-----------------------------------------------------------===//

LLVMLibFunction::LLVMLibFunction(std::string &_prote_name, size_t _proto_arg_nums, ScalarTypePtr _type,
                                 std::string &_name, ParamList &_list)
    : LibraryFunction(_type, _name, _list), proto_name(_prote_name), proto_arg_nums(_proto_arg_nums) {}

bool LLVMLibFunction::IsSYSYLibFunction() const { return false; }

std::string &LLVMLibFunction::GetProtoName() { return proto_name; }
size_t LLVMLibFunction::GetProtoArgNums() const { return proto_arg_nums; }

LLVMLibFuncPtr LLVMLibFunction::CreatePtr(std::string _prote_name, size_t _proto_arg_nums, ScalarTypePtr _type,
                                          std::string _name, ParamList &_list) {
    return std::make_shared<LLVMLibFunction>(_prote_name, _proto_arg_nums, _type, _name, _list);
}

std::ostream &operator<<(std::ostream &os, BaseFuncPtr func) {
    os << func->tollvmIR();
    return os;
}
