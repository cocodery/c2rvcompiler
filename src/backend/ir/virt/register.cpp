#include "backend/ir/virt/register.hh"

//
// instruction trace
//

void itrace::set_from(uop_general *from) {
    Assert(from_ == nullptr, "every virt reg should only have one define chance in SSA");
    from_ = from;
}

uop_general *itrace::from() const { return from_; }

void itrace::add_ref(uop_general *ref) { refs_.insert(ref); }

const std::unordered_set<uop_general *> &itrace::refs() const { return refs_; }

//
// call trace
//

void ctrace::set_param_pos(size_t pos) {
    if (pos < 8) callin_[pos] += 1;
}

ctrace::func ctrace::param_pos_info() const { return callin_; }

void ctrace::set_is_retval(bool flg) { is_retval_ = flg; }

bool ctrace::is_retval() const { return is_retval_; }

void ctrace::set_pstk(size_t idx) { pstk_ = idx; }

size_t ctrace::pstk() const { return pstk_; }

//
// live interval
//

void live_info::set_begin(size_t t) { begin_ = t; };

size_t live_info::begin() const { return begin_; }

void live_info::set_end(size_t t) { end_ = t; };

size_t live_info::end() const { return end_; }

//
// stack info if reg on stack
//

void stk_info::set_slen(size_t slen) { slen_ = slen; };

size_t stk_info::slen() const { return slen_; }

void stk_info::set_off(off64_t off) { off_ = off; };

off64_t stk_info::off() const { return off_; }

//
// alloc info
//

void alc_info::set_onstk(bool on) { onstk_ = on; };

bool alc_info::onstk() const { return onstk_; }

void alc_info::set_confirm(bool on) { confirm_ = on; };

bool alc_info::confirm() const { return confirm_; }

void alc_info::set_rregid(size_t inp) { rregid_ = inp; };

size_t alc_info::rregid() const { return rregid_; }

//
// virtual register
//

virt_reg::virt_reg(VREG_KIND kind, VREG_TYPE type, uxlen_t value)
    : itrace(), ctrace(), live_info(), kind_(kind), type_(type), value_(value) {}

VREG_TYPE virt_reg::type() const { return type_; }

VREG_KIND virt_reg::kind() const { return kind_; }

uxlen_t virt_reg::value() const { return value_; }

const std::unique_ptr<stk_info> &virt_reg::sinfo() const { return sinfo_; }

void virt_reg::set_sinfo(std::unique_ptr<stk_info> &info) { sinfo_ = std::move(info); }

size_t virt_reg::length() const {
    switch (type_) {
        case VREG_TYPE::FLT:
            return sizeof(float);
        case VREG_TYPE::INT:
            return sizeof(int);
        case VREG_TYPE::PTR:
            return sizeof(void *);
        case VREG_TYPE::ARR:
            Assert(sinfo_, "arr but stk info not set");
            return sizeof(int) * sinfo_->slen();
    }
    return 0;
}

//
// virtual register allocation
//

vr_allocor::vr_allocor() {}

virt_reg *vr_allocor::alloc_imm(uxlen_t value) {
    if (auto fnd = imm_map_.find(value); fnd != imm_map_.end()) {
        return fnd->second;
    }

    auto ptr = std::make_unique<virt_reg>(VREG_KIND::IMM, VREG_TYPE::INT, (int32_t)value);
    auto raw = ptr.get();
    imm_map_[value] = raw;

    Assert(storage_.find(raw) == storage_.end(), "alloc same addr ???");

    storage_[raw] = std::move(ptr);
    return raw;
}

virt_reg *vr_allocor::alloc_reg(VREG_TYPE type, uxlen_t idx) {
    Assert(type != VREG_TYPE::ARR, "reg type can not be arr");

    if (auto fnd = vr_map_.find(idx); fnd != vr_map_.end()) {
        return fnd->second;
    }

    auto ptr = std::make_unique<virt_reg>(VREG_KIND::REG, type, idx);
    auto raw = ptr.get();
    vr_map_[idx] = raw;

    Assert(storage_.find(raw) == storage_.end(), "alloc same addr ???");

    storage_[raw] = std::move(ptr);
    return raw;
}

virt_reg *vr_allocor::alloc_stk(VREG_TYPE type, uxlen_t slen) {
    auto sptr = std::make_unique<stk_info>();
    auto sraw = sptr.get();
    sptr->set_slen(slen);

    auto ptr = std::make_unique<virt_reg>(VREG_KIND::STK, type, (uxlen_t)sraw);
    auto raw = ptr.get();

    // sptr should not be used again
    ptr->set_onstk(true);
    ptr->set_sinfo(sptr);
    stk_map_[(uxlen_t)sraw] = raw;

    Assert(storage_.find(raw) == storage_.end(), "alloc same addr ???");

    storage_[raw] = std::move(ptr);
    return raw;
}

virt_reg *vr_allocor::alloc_loc(uxlen_t value) {
    if (auto fnd = fc_map_.find(value); fnd != fc_map_.end()) {
        return fnd->second;
    }

    auto ptr = std::make_unique<virt_reg>(VREG_KIND::LOC, VREG_TYPE::FLT, value);
    auto raw = ptr.get();
    fc_map_[value] = raw;

    Assert(storage_.find(raw) == storage_.end(), "alloc same addr ???");

    storage_[raw] = std::move(ptr);
    return raw;
}

virt_reg *vr_allocor::alloc_prm(VREG_TYPE type, uxlen_t idx, uxlen_t rid) {
    Assert(type != VREG_TYPE::ARR, "prm type can not be arr");

    if (auto fnd = vr_map_.find(idx); fnd != vr_map_.end()) {
        return fnd->second;
    }

    auto ptr = std::make_unique<virt_reg>(VREG_KIND::PRM, type, rid);
    auto raw = ptr.get();
    vr_map_[idx] = raw;

    Assert(storage_.find(raw) == storage_.end(), "alloc same addr ???");

    storage_[raw] = std::move(ptr);
    return raw;
}

virt_reg *vr_allocor::alloc_spec(uxlen_t idx) {
    if (auto fnd = spec_.find(idx); fnd != spec_.end()) {
        return fnd->second.get();
    }

    auto ptr = std::make_unique<virt_reg>(VREG_KIND::SPEC, VREG_TYPE::PTR, idx);
    auto raw = ptr.get();
    spec_[idx] = std::move(ptr);
    return raw;
}

virt_reg *vr_allocor::alloc_zero() {
    static virt_reg *zero = nullptr;

    if (zero != nullptr) {
        return zero;
    }

    auto ptr = std::make_unique<virt_reg>(VREG_KIND::ZERO, VREG_TYPE::PTR, 0);
    auto raw = ptr.get();
    zero = raw;
    return raw;
}

virt_reg *vr_allocor::get_reg(uxlen_t idx) { return vr_map_.at(idx); }

void vr_allocor::link(uxlen_t nw, uxlen_t old) { vr_map_[nw] = vr_map_[old]; }

void vr_allocor::give_loc(std::unordered_set<uint32_t> &locs) {
    for (auto &&pair : fc_map_) {
        locs.insert(pair.first);
    }
}
