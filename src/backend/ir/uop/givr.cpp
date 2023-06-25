#include "backend/ir/uop/uop.hh"
#include "backend/ir/virt/register.hh"

void uop_set_iparam::givr(std::unordered_set<virt_reg *> &vec) {
    if (rs_->kind() == VREG_KIND::REG) vec.insert(rs_);
}

void uop_set_fparam::givr(std::unordered_set<virt_reg *> &vec) {
    if (rs_->kind() == VREG_KIND::REG) vec.insert(rs_);
}

void uop_call::givr(std::unordered_set<virt_reg *> &vec) {
    if (retval_)
        if (retval_->kind() == VREG_KIND::REG) vec.insert(retval_);
}

void uop_li::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
}

void uop_mv::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (rs_->kind() == VREG_KIND::REG) vec.insert(rs_);
}

void uop_fmv::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (rs_->kind() == VREG_KIND::REG) vec.insert(rs_);
}

void uop_cvts2w::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (rs_->kind() == VREG_KIND::REG) vec.insert(rs_);
}

void uop_cvtw2s::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (rs_->kind() == VREG_KIND::REG) vec.insert(rs_);
}

void uop_b::givr(std::unordered_set<virt_reg *> &vec) {
    if (cond_->kind() == VREG_KIND::REG) vec.insert(cond_);
}

void uop_icmp_b::givr(std::unordered_set<virt_reg *> &vec) {
    if (lhs_->kind() == VREG_KIND::REG) vec.insert(lhs_);
    if (rhs_->kind() == VREG_KIND::REG) vec.insert(rhs_);
}

void uop_la::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
}

void uop_lla::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
}

void uop_ld::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (rb_->kind() == VREG_KIND::REG) vec.insert(rb_);
}

void uop_st::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (rb_->kind() == VREG_KIND::REG) vec.insert(rb_);
}

void uop_ld_l::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
}

void uop_st_l::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
}

void uop_fld::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (rb_->kind() == VREG_KIND::REG) vec.insert(rb_);
}

void uop_fst::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (rb_->kind() == VREG_KIND::REG) vec.insert(rb_);
}

void uop_fld_l::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
}

void uop_fst_l::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
}

void uop_fld_ll::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
}

void uop_icmp::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (lhs_->kind() == VREG_KIND::REG) vec.insert(lhs_);
    if (rhs_->kind() == VREG_KIND::REG) vec.insert(rhs_);
}

void uop_fcmp::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (lhs_->kind() == VREG_KIND::REG) vec.insert(lhs_);
    if (rhs_->kind() == VREG_KIND::REG) vec.insert(rhs_);
}

void uop_bin::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (lhs_->kind() == VREG_KIND::REG) vec.insert(lhs_);
    if (rhs_->kind() == VREG_KIND::REG) vec.insert(rhs_);
}

void uop_fbin::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (lhs_->kind() == VREG_KIND::REG) vec.insert(lhs_);
    if (rhs_->kind() == VREG_KIND::REG) vec.insert(rhs_);
}

void uop_ftri::givr(std::unordered_set<virt_reg *> &vec) {
    if (rd_->kind() == VREG_KIND::REG) vec.insert(rd_);
    if (lhs_->kind() == VREG_KIND::REG) vec.insert(lhs_);
    if (rhs_->kind() == VREG_KIND::REG) vec.insert(rhs_);
    if (ahs_->kind() == VREG_KIND::REG) vec.insert(ahs_);
}