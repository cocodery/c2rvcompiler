#include "rluop.hh"

#include "../../utils.hh"
#include "../riscv/riscv-cst.hh"

//
// set interval
//

void uop_ret::set_interval(size_t idx) {}

void uop_set_param::set_interval(size_t idx) {
    src->end = idx;
    src->ref += 1;
}

void uop_call::set_interval(size_t idx) {
    if (retval) {
        retval->begin = idx;
        retval->ref += 1;
        retval->from = this;

        retval->inrreg = riscv::a0;
    }
}

void uop_li::set_interval(size_t idx) {
    dst->begin = idx;
    dst->from = this;

    dst->inrreg = riscv::t0;
}

void uop_mv::set_interval(size_t idx) {
    src->end = idx;
    src->ref += 1;
    dst->begin = idx;
    dst->from = this;
}

void uop_cvts2w::set_interval(size_t idx) {
    src->end = idx;
    src->ref += 1;
    dst->begin = idx;
    dst->from = this;
}

void uop_cvtw2s::set_interval(size_t idx) {
    src->end = idx;
    src->ref += 1;
    dst->begin = idx;
    dst->from = this;
}

void uop_b::set_interval(size_t idx) {
    cond->ref += 1;
    cond->end = idx;
}

void uop_cmp_b::set_interval(size_t idx) {
    lhs->ref += 1;
    lhs->end = idx;
    rhs->ref += 1;
    rhs->end = idx;
}

void uop_j::set_interval(size_t idx) {}

void uop_la::set_interval(size_t idx) {
    dst->begin = idx;
    dst->from = this;

    dst->inrreg = riscv::t1;
}

void uop_load::set_interval(size_t idx) {
    base->end = idx;
    base->ref += 1;
    dst->begin = idx;
    dst->from = this;
}

void uop_load_tag::set_interval(size_t idx) {
    dst->begin = idx;
    dst->from = this;
}

void uop_store::set_interval(size_t idx) {
    base->end = idx;
    base->ref += 1;
    src->end = idx;
    src->ref += 1;
}

void uop_store_tag::set_interval(size_t idx) {
    src->end = idx;
    src->ref += 1;
}

void uop_cmp::set_interval(size_t idx) {
    lhs->end = idx;
    lhs->ref += 1;
    rhs->end = idx;
    rhs->ref += 1;
    dst->begin = idx;
    dst->from = this;
}

void uop_bin::set_interval(size_t idx) {
    lhs->end = idx;
    lhs->ref += 1;
    rhs->end = idx;
    rhs->ref += 1;
    dst->begin = idx;
    dst->from = this;
}

void uop_fbin::set_interval(size_t idx) {
    lhs->end = idx;
    lhs->ref += 1;
    rhs->end = idx;
    rhs->ref += 1;
    dst->begin = idx;
    dst->from = this;
}

void uop_ftri::set_interval(size_t idx) {
    op1->end = idx;
    op1->ref += 1;
    op2->end = idx;
    op2->ref += 1;
    op3->end = idx;
    op3->ref += 1;
    dst->begin = idx;
    dst->from = this;
}

//
// load immediate
//

std::unique_ptr<uop_general> uop_ret::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_set_param::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_call::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_li::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_mv::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_cvts2w::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_cvtw2s::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_b::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_cmp_b::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_j::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_la::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_load::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_load_tag::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_store::load_immediate(virt_reg_allocor &vraor) {
    if (src->kind == VREG_KIND::IMM) {
        auto nwvreg = vraor.allocREG(4);
        nwvreg->isflt = true;
        auto op = std::make_unique<uop_li>();
        op->dst = nwvreg;
        op->imm = src->value;
        src = nwvreg;
        return op;
    } else if (src->kind == VREG_KIND::LOC) {
        auto nwvreg = vraor.allocREG(4);
        nwvreg->isflt = true;
        auto op = std::make_unique<uop_load_tag>();
        op->onflt = true;
        op->dst = nwvreg;
        op->src = "LC." + std::to_string(src->value);
        src = nwvreg;
        return op;
    }
    return nullptr;
}

std::unique_ptr<uop_general> uop_store_tag::load_immediate(virt_reg_allocor &vraor) { return nullptr; }

std::unique_ptr<uop_general> uop_cmp::load_immediate(virt_reg_allocor &vraor) {
    if (onflt) {
        if (lhs->kind == VREG_KIND::LOC) {
            auto nwvreg = vraor.allocREG(4);
            nwvreg->isflt = true;
            auto op = std::make_unique<uop_load_tag>();
            op->onflt = true;
            op->dst = nwvreg;
            op->src = "LC." + std::to_string(lhs->value);
            lhs = nwvreg;
            return op;
        } else if (rhs->kind == VREG_KIND::LOC) {
            auto nwvreg = vraor.allocREG(4);
            nwvreg->isflt = true;
            auto op = std::make_unique<uop_load_tag>();
            op->onflt = true;
            op->dst = nwvreg;
            op->src = "LC." + std::to_string(rhs->value);
            rhs = nwvreg;
            return op;
        }
    } else {
        if (lhs->kind == VREG_KIND::IMM) {
            auto nwvreg = vraor.allocREG(4);
            auto op = std::make_unique<uop_li>();
            op->dst = nwvreg;
            op->imm = lhs->value;
            lhs = nwvreg;
            return op;
        } else if (rhs->kind == VREG_KIND::IMM && !imm_width_within(12, rhs->value)) {
            auto nwvreg = vraor.allocREG(4);
            auto op = std::make_unique<uop_li>();
            op->dst = nwvreg;
            op->imm = rhs->value;
            rhs = nwvreg;
            return op;
        }
    }
    return nullptr;
}

std::unique_ptr<uop_general> uop_bin::load_immediate(virt_reg_allocor &vraor) {
    if (lhs->kind == VREG_KIND::IMM) {
        if (imm_width_within(12, lhs->value)) {
            if (kind == IBIN_KIND::ADD || kind == IBIN_KIND::MUL) {
                auto tmp = lhs;
                lhs = rhs;
                rhs = tmp;
            }
        } else {
            auto nwvreg = vraor.allocREG(4);
            auto op = std::make_unique<uop_li>();
            op->dst = nwvreg;
            op->imm = lhs->value;
            lhs = nwvreg;
            return op;
        }
    } else if (rhs->kind == VREG_KIND::IMM && !imm_width_within(12, rhs->value)) {
        auto nwvreg = vraor.allocREG(4);
        auto op = std::make_unique<uop_li>();
        op->dst = nwvreg;
        op->imm = rhs->value;
        rhs = nwvreg;
        return op;
    } else if (rhs->kind == VREG_KIND::IMM && imm_width_within(12, rhs->value) && kind == IBIN_KIND::SUB) {
        kind = IBIN_KIND::ADD;
        rhs->value = -rhs->value;
    }
    return nullptr;
}

std::unique_ptr<uop_general> uop_fbin::load_immediate(virt_reg_allocor &vraor) {
    if (lhs->kind == VREG_KIND::LOC) {
        auto nwvreg = vraor.allocREG(4);
        nwvreg->isflt = true;
        auto op = std::make_unique<uop_load_tag>();
        op->onflt = true;
        op->dst = nwvreg;
        op->src = "LC." + std::to_string(lhs->value);
        lhs = nwvreg;
        return op;
    } else if (rhs->kind == VREG_KIND::LOC) {
        auto nwvreg = vraor.allocREG(4);
        nwvreg->isflt = true;
        auto op = std::make_unique<uop_load_tag>();
        op->onflt = true;
        op->dst = nwvreg;
        op->src = "LC." + std::to_string(rhs->value);
        rhs = nwvreg;
        return op;
    }
    return nullptr;
}

std::unique_ptr<uop_general> uop_ftri::load_immediate(virt_reg_allocor &vraor) { return nullptr; }
