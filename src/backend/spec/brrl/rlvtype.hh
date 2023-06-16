#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <string>
#include <sstream>
#include <list>

#include "rlenum.hh"

using width_t = size_t;
using uuid_t = size_t;

using xlen_t = int64_t;
using uxlen_t = uint64_t;


// 虚拟寄存器
struct virt_reg {
    // 用于指示虚拟寄存器值的来源
    VREG_KIND kind;

    // 寄存器长度
    // 目前最大作用是判断长度
    // 4 => 32
    // 8 => 64
    // 变量类型 **应该从操作上判断**
    width_t len = 0;

    // ZERO 寄存器该值为 0
    // IMM  寄存器该值为 立即数值 这里认为所有的浮点数常数都会提升为全局变量
    // REG  寄存器该值为 寄存器编号
    // STK  寄存器该值为 栈数据编号
    // GLB  寄存器该值为 全局变量编号
    // LOC  寄存器该值为 局部常量符号值，目前看来是单精度浮点数字面值
    // PRM  寄存器该值为 函数参数编号
    // FP, SP, RA 对应不同架构使用的寄存器
    uxlen_t value = 0;

    // 被使用次数
    size_t ref = 0;

    // 被设置位置
    size_t beset = 0;

    // 活跃区间
    size_t begin = 0;
    size_t end = 0;

    std::string to_string();
};

using virt_reg_ptr = std::unique_ptr<virt_reg>;

struct virt_stkinf {
    
    // 栈上数据的种类
    VSTK_KIND kind;

    // 栈上数据的长度
    width_t len;

    // 栈上偏移量

};

using virt_stkinf_ptr = std::unique_ptr<virt_stkinf>;

struct virt_reg_allocor {

    // 用于通过 uid 查阅虚拟寄存器的引用
    std::unordered_map<uuid_t, virt_reg *> map;

    // 用于通过浮点常量值查阅虚拟寄存器的引用
    std::unordered_map<uuid_t, virt_reg *> fcstmap;

    // 用于内存管理，保存虚拟寄存器资源
    std::list<virt_reg_ptr> storage;

    // 用于额外分配寄存器
    size_t nwidx_alloc = 0;

    virt_reg *alloc(VREG_KIND kind, width_t len, uxlen_t value);
    virt_reg *getREG(uuid_t uid);

    // alias lhs = rhs
    void link(uuid_t lhs, uuid_t rhs);

    // 分配新的额外寄存器
    virt_reg *allocREG(width_t len, uxlen_t value);
};

struct virt_stkinf_allocor {

    // 用于通过 uid 查阅栈信息的引用
    std::unordered_map<uuid_t, virt_stkinf *> map;

    // 用于内存管理，保存栈信息资源
    std::list<virt_stkinf_ptr> storage;

    uuid_t alloc(VSTK_KIND kind, width_t len);
    virt_stkinf *getSTK(uuid_t uid);

};