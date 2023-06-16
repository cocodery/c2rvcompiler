#pragma once

#include <string>

#include "rlvtype.hh"

struct uop_general {
    size_t uop_idx;

    virtual std::string to_string() = 0;
    virtual void set_interval(size_t idx) = 0;
};

struct uop_ret : public uop_general {

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_set_param : public uop_general {
    // 将 src 中的值保存到第 idx 个传参寄存器上
    virt_reg *src;

    size_t idx;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_call : public uop_general {
    // retval 是被调用函数 callee 的返回值
    // 默认保存在 abi 对应的第一个返回值寄存器中
    virt_reg *retval;

    std::string callee;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_li : public uop_general {
    // 将立即数加载进 dst
    xlen_t imm;

    virt_reg *dst;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_mv : public uop_general {
    // 寄存器间移动
    virt_reg *src;

    virt_reg *dst;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_cvts2w : public uop_general {
    // single to word 转换
    virt_reg *src;

    virt_reg *dst;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_cvtw2s : public uop_general {
    // word to single 转换
    virt_reg *src;

    virt_reg *dst;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_b : public uop_general {
    virt_reg *cond;

    std::string pfx;
    size_t lbid;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_j : public uop_general {
    std::string pfx;
    size_t lbid;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_la : public uop_general {
    // 加载地址
    virt_reg *dst;

    std::string src;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_load : public uop_general {
    virt_reg *dst;
    virt_reg *base;
    xlen_t off;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_load_tag : public uop_general {
    virt_reg *dst;

    std::string src;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_store : public uop_general {
    virt_reg *src;
    virt_reg *base;
    xlen_t off;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_store_tag : public uop_general {
    virt_reg *src;
    virt_reg *tgt;

    std::string dst;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_cmp : public uop_general {
    bool onflt;
    COMP_KIND kind;

    virt_reg *lhs;
    virt_reg *rhs;
    virt_reg *dst;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_bin : public uop_general {
    IBIN_KIND kind;

    virt_reg *dst;
    virt_reg *lhs;
    virt_reg *rhs;

    std::string to_string();
    void set_interval(size_t idx);
};

struct uop_fbin : public uop_general {
    FBIN_KIND kind;
    
    virt_reg *dst;
    virt_reg *lhs;
    virt_reg *rhs;

    std::string to_string();
    void set_interval(size_t idx);
};