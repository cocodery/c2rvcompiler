#include <codegen.hh>

#include <unordered_set>

namespace backend {

using InterferGraph = std::unordered_map<size_t, std::unordered_set<size_t>>;

struct LiveIO {
    std::unordered_set<size_t> livein;
    std::unordered_set<size_t> liveout;
};

auto analysisBB(CfgNodePtr &cfgnode, LiveIO &liveinfo, InterferGraph &ig) {
    auto &&ilist = cfgnode->GetInstList();
    std::unordered_map<size_t, bool> curliveinfo;
    for (auto &&liveo: liveinfo.liveout) {
        curliveinfo[liveo] = true;
    }

    for (auto riter = ilist.rbegin(); riter != ilist.rend(); riter++) {
        auto &&oprands = (*riter)->GetOprands();
        auto &&result = (*riter)->GetResult();

        if (oprands.size()) {
            for (auto &&op: oprands) {
                if (op->IsConstant() && !op->GetBaseType()->IsArray())
                    continue;
                curliveinfo[op->guidx()] = true;
            }
        }
        if (result != nullptr) {
            curliveinfo[result->guidx()] = false;
        }

        for (auto lhsiter = curliveinfo.begin(); lhsiter != curliveinfo.end(); ) {
            auto lhs = lhsiter;
            lhsiter++;
            if (!lhs->second) {
                continue;
            }
            for (auto rhsiter = lhsiter; rhsiter != curliveinfo.end(); ++rhsiter) {
                auto &&rhs = rhsiter;
                if (!rhs->second) {
                    continue;
                }
                ig[lhs->first].insert(rhs->first);
                ig[rhs->first].insert(lhs->first);
            }
        }
    }

    for (auto iter = curliveinfo.begin(); iter != curliveinfo.end(); ++iter) {
        if (iter->second) {
            liveinfo.livein.insert(iter->first);
        }
    }

}

void CodeGen::GenBB() {
    auto &&gvals = comp_unit.getGlbTable();
    // GValToBBs(gvals);
    for (auto &&func: comp_unit.GetNormalFuncTable()) {
        FuncToBBs(func, gvals);
    }
}

auto CodeGen::GValToBBs(SymbolTable &gvals) -> std::shared_ptr<ASMBasic> {
    return nullptr;
}

auto CodeGen::FuncToBBs(NormalFuncPtr &func, SymbolTable &gvals) -> std::shared_ptr<ASMBasic> {
    auto prog = std::make_shared<Progress>();

    auto &&entry = func->GetEntryNode();
    prog->name = func->GetFuncName();

    std::list<ASMBasicBlock> abb;
    std::unordered_map<size_t, LiveIO> liveio;
    InterferGraph ig;

    for (auto &&node: func->TopoSortFromExit()) {
        auto bidx = node->GetBlkIdx();
        auto curliveio = LiveIO();
        for (auto &&[key, gval]: gvals.GetNameValueMap()) {
            curliveio.liveout.insert(gval->guidx());
        }
        for (auto &&suc: node->GetSuccessors()) {
            auto sucidx = suc->GetBlkIdx();
            for (auto &&livei: liveio[sucidx].livein) {
                curliveio.liveout.insert(livei);
            }
        }
        analysisBB(node, curliveio, ig);
        liveio[bidx] = curliveio;
    }

    // std::unordered_map<size_t, bool> pmp;
    // for (auto &&[k, v]: ig) {
    //     std::cout << k << std::endl;
    //     pmp[k] = false;
    // }
    // std::cout.flush();
    // for (auto &&[k, v]: ig) {
    //     pmp[k] = true;
    //     for (auto &&p: v) {
    //         if (pmp[p])
    //             continue;
    //         std::cout << k << "," << p << std::endl;
    //     }
    // }

    return nullptr;
}

}