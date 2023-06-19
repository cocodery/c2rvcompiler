#include "lltorl.hh"

void rl_lltorl_instance::gencode() {
    for (auto &&bb : rlp->bbs) {
        auto lst = std::make_unique<ABBProg::ABBlock>();
        lst->tagname = bb->tag + std::to_string(bb->lbid);
        for (auto &&op : bb->ops) {
            op->gen_asm(lst);
        }
        prog_->abbs.push_back(std::move(lst));
    }

    for (auto &&loc : rlp->vreg_alloc.fcstmap) {
        auto var = std::make_unique<ABBGValue>();
        var->totalsiz = 1;
        var->name = "LC." + std::to_string(loc.second->value);
        var->isUninit = false;
        var->values.push_back(loc.second->value);
        prog_->locs.push_back(std::move(var));
    }
}