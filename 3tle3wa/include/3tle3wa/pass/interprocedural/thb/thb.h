#pragma once

#include "3tle3wa/ir/IR.hh"

#include <list>
#include <vector>


struct cmp {
    bool operator()(const BaseValuePtr a, const BaseValuePtr b) const{
        if (a == nullptr && b == nullptr) {
            return false;
        }
        auto flag1 = a->IsConstant();
        auto flag2 = b->IsConstant();
        if (!flag1 && !flag2) {
            return a->guidx() < b->guidx();
        }
        if (!flag1 && flag2) {
            return false;
        }
        if (flag1 && !flag2) {
            return true;
        }
        if (flag1 && flag2) {
            auto constant_a = static_cast<const Constant*>(a.get());
            auto constant_b = static_cast<const Constant*>(b.get());
            return constant_a->GetValue() < constant_b->GetValue();
        }
    }
};

namespace THB {
    void THB(const NormalFuncPtr&);

    void convert(std::map<BaseValuePtr, int, cmp> operands, std::vector<InstPtr> addInsts);
}
