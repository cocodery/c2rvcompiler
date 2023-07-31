#include "3tle3wa/ir/function/function.hh"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>

#include "3tle3wa/ir/function/basicblock.hh"
#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/instruction/controlFlowInst.hh"
#include "3tle3wa/ir/instruction/instruction.hh"
#include "3tle3wa/ir/value/type/baseType.hh"
#include "3tle3wa/utils/logs.hh"

//===-----------------------------------------------------------===//
//                     NormalFunction Implementation
//===-----------------------------------------------------------===//

NormalFunction::NormalFunction(ScalarTypePtr _type, std::string &_name, ParamList &_list)
    : BaseFunction(_type, _name, _list), structure(nullptr) {}

bool NormalFunction::IsLibFunction() const { return false; }

CfgNodePtr NormalFunction::CreateEntry() {
    entry = CtrlFlowGraphNode::CreatePtr(this, BlkAttr::Entry | BlkAttr::Normal);
    return entry;
}

CfgNodePtr NormalFunction::CreateExit() {
    exit = CtrlFlowGraphNode::CreatePtr(this, BlkAttr::Exit | BlkAttr::Normal);
    return exit;
}

CfgNodePtr NormalFunction::CreateCfgNode(BlkAttr::BlkType blk_type) {
    return CtrlFlowGraphNode::CreatePtr(this, blk_type);
}

CfgNodePtr NormalFunction::GetEntryNode() { return entry; }
CfgNodePtr NormalFunction::GetExitNode() { return exit; }

void NormalFunction::SetEntryNode(CfgNodePtr _entry) { entry = _entry; }
void NormalFunction::SetExitNode(CfgNodePtr _exit) { exit = _exit; }

void NormalFunction::GetCondBlks(CfgNodePtr &node, std::stack<CfgNodePtr> &parent, CfgNodeList &list,
                                 std::unordered_map<CtrlFlowGraphNode *, bool> &visit) {
    assert(node->blk_attr.cond_begin);  // node must be cond-begin
    std::stack<CfgNodePtr> stack;
    stack.push(node);

    while (!stack.empty()) {
        auto &&top = stack.top();
        stack.pop();

        if (!visit[top.get()]) {
            visit[top.get()] = true;
            list.push_back(top);

            Instruction *last_inst = top->GetLastInst().get();
            if (last_inst->IsBranchInst()) {
                BranchInst *br_inst = static_cast<BranchInst *>(last_inst);
                auto &&lhs = br_inst->GetTrueTarget();
                auto &&rhs = br_inst->GetFalseTarget();

                auto &&lhs_attr = lhs->blk_attr;
                auto &&rhs_attr = rhs->blk_attr;

                if (!rhs_attr.iftrue_begin && !rhs_attr.iffalse_begin && !rhs_attr.body_begin &&
                    !rhs_attr.structure_out) {
                    stack.push(rhs);
                }
                if (!lhs_attr.iftrue_begin && !lhs_attr.iffalse_begin && !lhs_attr.body_begin &&
                    !lhs_attr.structure_out) {
                    stack.push(lhs);
                }
                // last branch of cond
                if ((lhs_attr.iftrue_begin && rhs_attr.iffalse_begin) ||
                    (lhs_attr.body_begin && rhs_attr.structure_out)) {
                    parent.push(rhs);
                    parent.push(lhs);
                }
            } else {
                assert(false);
            }
        }
    }
    return;
}

void NormalFunction::GetBodyBlks(CfgNodePtr &node, CfgNodeList &list,
                                 std::unordered_map<CtrlFlowGraphNode *, bool> &visit) {
    std::stack<CfgNodePtr> stack;
    stack.push(node);

    auto &&ChkAllPredVisit = [&visit](CtrlFlowGraphNode *node) -> bool {
        if (node->blk_attr.CheckBlkType(BlkAttr::Entry)) return true;
        assert(node->GetPredecessors().size() > 0);
        for (auto &&pred : node->GetPredecessors()) {
            if (!visit[pred.get()]) {
                return false;
            }
        }
        return true;
    };

    while (!stack.empty()) {
        auto &&top = stack.top();
        stack.pop();

        if (!visit[top.get()] && ChkAllPredVisit(top.get()) && !top->blk_attr.CheckBlkType(BlkAttr::Exit)) {
            visit[top.get()] = true;
            list.push_back(top);

            Instruction *last_inst = top->GetLastInst().get();
            if (last_inst->IsBranchInst()) {
                BranchInst *br_inst = static_cast<BranchInst *>(last_inst);
                auto &&lhs = br_inst->GetTrueTarget();
                auto &&rhs = br_inst->GetFalseTarget();

                auto &&lhs_attr = lhs->blk_attr;
                auto &&rhs_attr = rhs->blk_attr;

                if (!rhs_attr.cond_begin && !rhs_attr.structure_out && !rhs_attr.iftrue_begin &&
                    !rhs_attr.iffalse_begin) {
                    stack.push(rhs);
                }
                if (!lhs_attr.cond_begin && !lhs_attr.structure_out && !lhs_attr.iftrue_begin &&
                    !lhs_attr.iffalse_begin) {
                    stack.push(lhs);
                }
            } else if (last_inst->IsJumpInst()) {
                JumpInst *jump_inst = static_cast<JumpInst *>(last_inst);
                auto &&target = jump_inst->GetTarget();

                auto &&target_attr = target->blk_attr;

                if (!target_attr.cond_begin && !target_attr.structure_out) {
                    stack.push(target);
                } else if (target_attr.cond_begin) {
                    GetCondBlks(target, stack, list, visit);
                } else if (target_attr.structure_out &&
                           (top->blk_attr.body_end || top->blk_attr.iftrue_end || top->blk_attr.iffalse_end)) {
                    stack.push(target);
                }
            }
        }
    }
    return;
}

CfgNodeList NormalFunction::GetSequentialNodes() {
    CfgNodeList list;
    std::unordered_map<CtrlFlowGraphNode *, bool> visit;

    GetBodyBlks(entry, list, visit);
    list.push_back(exit);

    return list;
}

CfgNodeList NormalFunction::GetReverseSeqNodes() {
    CfgNodeList list = GetSequentialNodes();
    list.reverse();

    return list;
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

    for (auto &&node : GetSequentialNodes()) {
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
