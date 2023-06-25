#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "3tle3wa/backend/asm/progress.hh"
#include "3tle3wa/backend/asm/riscv/asm.hh"
#include "3tle3wa/backend/asm/riscv/def.hh"
#include "3tle3wa/backend/interface.hh"
#include "3tle3wa/backend/ir/def.hh"
#include "3tle3wa/backend/utils.hh"
#include "3tle3wa/utils/Logs.hh"

class uop_general;
class pblock;
class rl_progress;

struct spack {
    bool t1{false};
    bool t2{false};
    bool s2{false};
};

// instruction trace 指令追踪
class itrace {
   protected:
    uop_general *from_{nullptr};
    std::unordered_set<uop_general *> refs_{};

   public:
    itrace() = default;

    virtual void set_from(uop_general *from) final;
    virtual uop_general *from() const final;

    virtual void add_ref(uop_general *ref) final;
    virtual const std::unordered_set<uop_general *> &refs() const final;

    virtual ~itrace() = default;
};

// call trace 调用追踪
class ctrace {
   private:
    typedef const size_t (&func)[8 + 1];

   protected:
    // 0 - 7 for reg, 8 for stack
    size_t callin_[8 + 1] = {0};

    bool is_retval_{false};

    size_t pstk_{0};

   public:
    ctrace() = default;

    void set_param_pos(size_t pos);
    func param_pos_info() const;

    void set_is_retval(bool flg);
    bool is_retval() const;

    void set_pstk(size_t flg);
    size_t pstk() const;

    virtual ~ctrace() = default;
};

// 活跃区间
class live_info {
   protected:
    size_t begin_{0};
    size_t end_{0};

   public:
    live_info() = default;

    bool stkpp_{false};

    void set_begin(size_t t);
    size_t begin() const;

    void set_end(size_t t);
    size_t end() const;

    virtual ~live_info() = default;
};

class stk_info {
   protected:
    // 数组为能存下多少内容
    // 标量为占据多少空间
    size_t slen_{0};

    int64_t off_{0};

   public:
    stk_info() = default;

    void set_slen(size_t slen);
    size_t slen() const;

    void set_off(int64_t off);
    int64_t off() const;

    virtual ~stk_info() = default;
};

class alc_info {
   protected:
    bool onstk_{false};
    bool confirm_{false};

    size_t rregid_{0};

   public:
    void set_onstk(bool on);
    bool onstk() const;

    void set_confirm(bool on);
    bool confirm() const;

    void set_rregid(size_t inp);
    size_t rregid() const;

    virtual ~alc_info() = default;
};

// 虚拟寄存器
class virt_reg : public itrace, public ctrace, public live_info, public serializable, public alc_info {
    VREG_KIND kind_{VREG_KIND::ZERO};
    VREG_TYPE type_{VREG_TYPE::PTR};

    // ZERO 寄存器该值为 0
    // IMM  寄存器该值为 立即数值 这里认为所有的浮点数常数都会提升为全局变量
    // REG  寄存器该值为 寄存器编号
    // STK  寄存器该值为 栈数据编号
    // LOC  寄存器该值为 局部常量符号值，目前看来是单精度浮点数字面值
    // PRM  寄存器该值为 函数参数编号
    uxlen_t value_{0};

    std::unique_ptr<stk_info> sinfo_{nullptr};

   public:
    virt_reg(VREG_KIND kind, VREG_TYPE type, uxlen_t value);

    VREG_TYPE type() const;
    VREG_KIND kind() const;
    uxlen_t value() const;
    size_t length() const;

    const std::unique_ptr<stk_info> &sinfo() const;
    void set_sinfo(std::unique_ptr<stk_info> &info);

    void format_str(FILE *fp);

    size_t load(pblock *pb, spack &spk, size_t to = 0);
    size_t store_where();
    void store(pblock *pb, size_t to = 0);
};

// 虚拟寄存器分配
class vr_allocor {
    // 虚拟寄存器表
    std::unordered_map<uxlen_t, virt_reg *> vr_map_;

    // 浮点常量表
    std::unordered_map<uxlen_t, virt_reg *> fc_map_;

    // 立即数表
    std::unordered_map<uxlen_t, virt_reg *> imm_map_;

    // 栈信息表
    std::unordered_map<uxlen_t, virt_reg *> stk_map_;

    // 存储资源
    std::unordered_map<virt_reg *, std::unique_ptr<virt_reg>> storage_;

    // 指定寄存器
    // 架构相关
    std::unordered_map<uxlen_t, std::unique_ptr<virt_reg>> spec_;

   public:
    vr_allocor();

    virt_reg *alloc_imm(uxlen_t value);
    virt_reg *alloc_reg(VREG_TYPE type, uxlen_t idx);
    virt_reg *alloc_stk(VREG_TYPE type, uxlen_t slen = 0);
    virt_reg *alloc_loc(uxlen_t value);
    virt_reg *alloc_prm(VREG_TYPE type, uxlen_t idx, uxlen_t rid);
    virt_reg *alloc_spec(uxlen_t idx);
    virt_reg *alloc_zero();

    virt_reg *get_reg(uxlen_t idx);

    void link(uxlen_t nw, uxlen_t old);

    void give_loc(std::unordered_set<uint32_t> &locs);

    void prinfo(std::fstream &fs);

    //
    // hi                                                                                          lo
    // +----+----+----+----+--------+--------------------------+-------------------------+----+----+
    // | ax | a8 | ra | fp | spills | for scalar aligned by 16 | for array aligned by 16 | ax | a8 |
    // +----+----+----+----+--------+--------------------------+-------------------------+----+----+
    //           ^ fp                                                                              ^ sp
    //
    // stack plan like this
    // assuming scalar var use offset index more
    // and array use reg index more
    //
    void plan_stack();

    /**
     * for integer registers
     *
     * ra, sp, gp, tp, fp for spec purpose
     *
     * t0 for li
     * t1 for la
     * t2 for sw/sd/flw/fsw with tag
     * a0 for retval
     * a0 - a7 for arg
     * t1, t2 for tmp l/s
     *
     * other 14 reg for common use
     *
     * for float registers
     *
     * fa0 for retval
     * fa0 - fa7 for arg
     * fs0, fs1 for tmp l/s
     * other 22 reg for common use
     *
     * for now we just use static way to assign regs
     * for better assignment, some regs that won't be use or free in some interval
     * could be assign to some virtual regs
     */
    void plan_reg(rl_progress &rlp);

   public:
    size_t total_stk_len{0};
    size_t ex_argl{0};
};