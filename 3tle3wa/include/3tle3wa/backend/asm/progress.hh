#pragma once

#include <fstream>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "3tle3wa/backend/asm/inst.hh"
#include "3tle3wa/backend/utils.hh"

class progress;
class glb_value;
struct aov_node;

// spec
class rv_inst;

using aov_nptr = std::unique_ptr<aov_node>;

struct aov_node {
    rv_inst *cur_{nullptr};
    uint64_t issue_need_{0};
    uint64_t write_reg_{0};
    std::unordered_set<aov_node *> prevs_;
    std::unordered_set<aov_node *> posts_;
};

// asm basic block progress block
class pblock {
    // 全局块号
    size_t lbidx_;

    // 块内的指令
    std::list<std::unique_ptr<asm_inst>> insts_;

    std::list<asm_inst *> inst_view_;

    std::list<aov_nptr> aov_source;

    void opm_rm_needless_ls();
    void opm_rm_needless_li();

    void opm_reorder();

   public:
    pblock(size_t lbidx, size_t reserve, progress *father);

    const progress *father_;

    void gen_asm(std::fstream &fs);

    void push(asm_inst *inst);

    void do_optimize();

    std::list<std::unique_ptr<asm_inst>> &ilst();
};

using ret_elem = std::pair<std::list<std::unique_ptr<pblock>>::iterator, std::list<std::unique_ptr<asm_inst>>::iterator>;

// asm basic block progress
class progress {
    // 过程标签
    std::string label_;

    // 过程内基本块
    std::list<std::unique_ptr<pblock>> pblks_;

    const std::unordered_map<size_t, glb_value *> &gname_map_;

   public:
    progress(std::string label, size_t reserve, const std::unordered_map<size_t, glb_value *> &gname_map);

    void gen_asm(std::fstream &fs);

    void push(std::unique_ptr<pblock> &pblk);

    void do_optimize();

    glb_value *ginfo(size_t gidx) const;

    std::unique_ptr<pblock> &front();
    std::unique_ptr<pblock> &back();
    std::vector<ret_elem> tails();
};
