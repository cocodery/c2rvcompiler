#include "lltorl.hh"

void rl_lltorl_instance::pinfo() {
    fprintf(stdout, "local constants:\n");
    for (auto &pair : rlp->vreg_alloc.fcstmap) {
        fprintf(stdout, "LC%ld: %f\n", pair.first, *(float *)(uxlen_t *)&pair.second->value);
    }
    fputc('\n', stdout);

    fprintf(stdout, "register info:\n");
    for (auto &reg : rlp->vreg_alloc.storage) {
        if (reg->kind == VREG_KIND::REG || reg->kind == VREG_KIND::PRM) {
            fprintf(stdout, "%%Reg_%ld@%ld:", reg->value, reg->len);
            fprintf(stdout, "\t(%ld, %ld).ref = %ld", reg->begin, reg->end, reg->ref);
            fputc('\n', stdout);
        }
    }
    fputc('\n', stdout);

    fprintf(stdout, "stack info:\n");
    for (auto &reg : rlp->vski_alloc.storage) {
        fprintf(stdout, "0x%lx: (%d, %ld)\n", (intptr_t)reg.get(), (unsigned)reg->kind, reg->len);
    }
    fputc('\n', stdout);
}

void rl_lltorl_instance::pir() {
    fprintf(stdout, "%s:\n", rlp->pnm.c_str());
    for (auto &bb : rlp->bbs) {
        fprintf(stdout, "%s%lu:\n", bb->tag.c_str(), bb->lbid);
        for (auto &&op : bb->ops) {
            fprintf(stdout, "%lu%s", op->uop_idx, op->to_string().c_str());
        }
    }
    fputc('\n', stdout);
}