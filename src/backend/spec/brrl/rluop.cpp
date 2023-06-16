#include "rluop.hh"

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
    }
}

void uop_li::set_interval(size_t idx) { dst->begin = idx; }

void uop_mv::set_interval(size_t idx) {
    src->end = idx;
    src->ref += 1;
    dst->begin = idx;
}

void uop_cvts2w::set_interval(size_t idx) {
    src->end = idx;
    src->ref += 1;
    dst->begin = idx;
}

void uop_cvtw2s::set_interval(size_t idx) {
    src->end = idx;
    src->ref += 1;
    dst->begin = idx;
}

void uop_b::set_interval(size_t idx) {
    cond->ref += 1;
    cond->end = idx;
}

void uop_j::set_interval(size_t idx) {}

void uop_la::set_interval(size_t idx) { dst->begin = idx; }

void uop_load::set_interval(size_t idx) {
    base->end = idx;
    base->ref += 1;
    dst->begin = idx;
}

void uop_load_tag::set_interval(size_t idx) { dst->begin = idx; }

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
}

void uop_bin::set_interval(size_t idx) {
    lhs->end = idx;
    lhs->ref += 1;
    rhs->end = idx;
    rhs->ref += 1;
    dst->begin = idx;
}

void uop_fbin::set_interval(size_t idx) {
    lhs->end = idx;
    lhs->ref += 1;
    rhs->end = idx;
    rhs->ref += 1;
    dst->begin = idx;
}