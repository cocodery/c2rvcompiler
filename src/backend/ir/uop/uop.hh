#pragma once

#include <string>
#include <string_view>

#include "../../asm/progress.hh"
#include "../../interface.hh"
#include "../virt/register.hh"

#define GETTER_SETTER(name, type)                \
   protected:                                    \
    type name##_{};                              \
                                                 \
   public:                                       \
    void set_##name(type inp) { name##_ = inp; } \
    type &get_##name() { return name##_; }       \
                                                 \
   private:

class uop_general : public serializable {
    GETTER_SETTER(uop_idx, size_t)

   public:
    virtual void trace_call() {}
    virtual void trace_inst() {}
    virtual void live_info() {}
    virtual void toasm(pblock *pb) = 0;
};

class uop_ret : public uop_general {
   public:
    void format_str(FILE *fp);

   public:
    void toasm(pblock *pb);
};

class uop_set_iparam : public uop_general {
    GETTER_SETTER(rs, virt_reg *)
    GETTER_SETTER(idx, size_t)
    GETTER_SETTER(pstk, size_t)

   public:
    void format_str(FILE *fp);

   public:
    void trace_call();
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_set_fparam : public uop_general {
    GETTER_SETTER(rs, virt_reg *)
    GETTER_SETTER(idx, size_t)
    GETTER_SETTER(pstk, size_t)

   public:
    void format_str(FILE *fp);

   public:
    void trace_call();
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_call : public uop_general {
    // retval 是被调用函数 callee 的返回值
    // 默认保存在 abi 对应的第一个返回值寄存器中
    GETTER_SETTER(retval, virt_reg *);
    GETTER_SETTER(callee, const char *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_call();
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_li : public uop_general {
    GETTER_SETTER(imm, xlen_t);
    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_mv : public uop_general {
    GETTER_SETTER(rs, virt_reg *);
    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_fmv : public uop_general {
    GETTER_SETTER(rs, virt_reg *);
    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_cvts2w : public uop_general {
    GETTER_SETTER(rs, virt_reg *);
    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_cvtw2s : public uop_general {
    GETTER_SETTER(rs, virt_reg *);
    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_b : public uop_general {
    GETTER_SETTER(cond, virt_reg *);
    GETTER_SETTER(lbid, size_t);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_icmp_b : public uop_general {
    GETTER_SETTER(kind, COMP_KIND);

    GETTER_SETTER(lhs, virt_reg *);
    GETTER_SETTER(rhs, virt_reg *);

    GETTER_SETTER(lbid, size_t);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_j : public uop_general {
    GETTER_SETTER(lbid, size_t);

   public:
    void format_str(FILE *fp);

   public:
    void toasm(pblock *pb);
};

class uop_la : public uop_general {
    GETTER_SETTER(glb_idx, size_t);
    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_lla : public uop_general {
    GETTER_SETTER(glb_idx, size_t);
    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_ld : public uop_general {
    GETTER_SETTER(rd, virt_reg *);
    GETTER_SETTER(rb, virt_reg *);
    GETTER_SETTER(off, xlen_t);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_st : public uop_general {
    GETTER_SETTER(rd, virt_reg *);
    GETTER_SETTER(rb, virt_reg *);
    GETTER_SETTER(off, xlen_t);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_ld_l : public uop_general {
    GETTER_SETTER(rd, virt_reg *);
    GETTER_SETTER(glb_idx, size_t);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_st_l : public uop_general {
    GETTER_SETTER(rd, virt_reg *);
    GETTER_SETTER(glb_idx, size_t);
    GETTER_SETTER(rt, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_fld : public uop_general {
    GETTER_SETTER(rd, virt_reg *);
    GETTER_SETTER(rb, virt_reg *);
    GETTER_SETTER(off, xlen_t);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_fst : public uop_general {
    GETTER_SETTER(rd, virt_reg *);
    GETTER_SETTER(rb, virt_reg *);
    GETTER_SETTER(off, xlen_t);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_fld_l : public uop_general {
    GETTER_SETTER(rd, virt_reg *);
    GETTER_SETTER(glb_idx, size_t);
    GETTER_SETTER(rt, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_fst_l : public uop_general {
    GETTER_SETTER(rd, virt_reg *);
    GETTER_SETTER(glb_idx, size_t);
    GETTER_SETTER(rt, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_fld_ll : public uop_general {
    GETTER_SETTER(rd, virt_reg *);
    GETTER_SETTER(loc_idx, size_t);
    GETTER_SETTER(rt, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_icmp : public uop_general {
    GETTER_SETTER(kind, COMP_KIND);

    GETTER_SETTER(lhs, virt_reg *);
    GETTER_SETTER(rhs, virt_reg *);

    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_fcmp : public uop_general {
    GETTER_SETTER(kind, COMP_KIND);

    GETTER_SETTER(lhs, virt_reg *);
    GETTER_SETTER(rhs, virt_reg *);

    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_bin : public uop_general {
    GETTER_SETTER(kind, IBIN_KIND);

    GETTER_SETTER(lhs, virt_reg *);
    GETTER_SETTER(rhs, virt_reg *);

    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_fbin : public uop_general {
    GETTER_SETTER(kind, FBIN_KIND);

    GETTER_SETTER(lhs, virt_reg *);
    GETTER_SETTER(rhs, virt_reg *);

    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};

class uop_ftri : public uop_general {
    GETTER_SETTER(kind, FTRI_KIND);

    GETTER_SETTER(lhs, virt_reg *);
    GETTER_SETTER(rhs, virt_reg *);
    GETTER_SETTER(ahs, virt_reg *);

    GETTER_SETTER(rd, virt_reg *);

   public:
    void format_str(FILE *fp);

   public:
    void trace_inst();
    void live_info();
    void toasm(pblock *pb);
};