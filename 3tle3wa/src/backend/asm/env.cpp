#include "3tle3wa/backend/asm/env.hh"

#include "3tle3wa/backend/ir/uop/uop.hh"
#include "3tle3wa/backend/xin/xin.hh"

static bool declarr(ConstArrayPtr &bvaptr, std::unique_ptr<glb_value> &result) {
    bool uninit = true;
    auto &&cstarr = bvaptr->GetConstArr();
    for (auto &&v : cstarr) {
        auto pk = xcval(v->GetValue());

        // 目前不检查，认为是一定 32 bit
        if (pk.v32 != 0) {
            uninit = false;
        } 
        result->push(pk.v32);
    }
    return uninit;
}

void asm_env::make_gvals(GlobalValuePtr &gvptr, const std::string &name) {
    auto &&initval = gvptr->GetInitValue();

    if (initval->IsUnInitVar()) {
        auto &&uninit_ptr = std::dynamic_pointer_cast<UnInitVar>(initval);
        Assert(uninit_ptr, "bad dynamic cast");

        if (uninit_ptr->GetBaseType()->IsArray()) {
            auto &&uninit_arrty_ptr = std::dynamic_pointer_cast<ListType>(initval->GetBaseType());
            Assert(uninit_arrty_ptr, "bad dynamic cast");
            auto gval = std::make_unique<glb_value>(name, uninit_arrty_ptr->GetCapacity(), true, 0);
            gname_map_[gvptr->GetGlobalValueIdx()] = gval.get();
            gvals_.push_back(std::move(gval));
            return;
        }

        auto gval = std::make_unique<glb_value>(name, 1, true, 0);
        gname_map_[gvptr->GetGlobalValueIdx()] = gval.get();
        gvals_.push_back(std::move(gval));
        return;
    }

    if (gvptr->GetBaseType()->IsArray()) {
        auto &&arr_ptr = std::dynamic_pointer_cast<ConstArray>(gvptr->GetInitValue());
        Assert(arr_ptr, "bad dynamic cast");

        auto &&arrty_ptr = std::dynamic_pointer_cast<ListType>(arr_ptr->GetBaseType());
        Assert(arrty_ptr, "bad dynamic cast");

        auto gval = std::make_unique<glb_value>(name, arrty_ptr->GetCapacity(), false, arr_ptr->GetConstArr().size());
        gval->uninit_ = declarr(arr_ptr, gval);
        gname_map_[gvptr->GetGlobalValueIdx()] = gval.get();
        gvals_.push_back(std::move(gval));
        return;
    }

    auto &&val_ptr = std::dynamic_pointer_cast<Constant>(gvptr->GetInitValue());
    auto pk = xcval(val_ptr->GetValue());
    auto gval = std::make_unique<glb_value>(name, 1, false, 1);
    gname_map_[gvptr->GetGlobalValueIdx()] = gval.get();
    gval->push(pk.v32);
    gvals_.push_back(std::move(gval));
}

void asm_env::make_prog(NormalFuncList &flst) {
    std::vector<std::unique_ptr<cross_internal_manager>> xinmgrs;
    std::vector<std::unique_ptr<std::thread>> trds;

    for (auto &&fptr : flst) {
        auto xin = std::make_unique<cross_internal_manager>(fptr, lc_pool_, gname_map_);
        auto bear_xin = xin.get();
        auto trd = std::make_unique<std::thread>([bear_xin]() -> void { bear_xin->do_compile(); });
        xinmgrs.push_back(std::move(xin));
        trds.push_back(std::move(trd));
    }

    for (auto &&trd : trds) {
        trd->join();
    }

    for (auto &&xinmgr : xinmgrs) {
        if (xinmgr->apg_ != nullptr) {
            pgrs_.push_back(std::move(xinmgr->apg_));
        }
    }
}

const char *skip = "integer-divide-optimization-3";

void asm_env::gen_asm(std::fstream &fs) {

    // no pic
    fs << "\t.option nopic" << std::endl;
    // arch info
    fs << "\t.attribute arch, \"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0\"" << std::endl;
    // aligned access
    fs << "\t.attribute unaligned_access, 0" << std::endl;
    // stack aligned
    fs << "\t.attribute stack_align, 16" << std::endl;

    fs << std::endl;

    if (not gvals_.empty()) {
        for (auto &&gval : gvals_) {
            gval->gen_asm(fs);
            fs << std::endl;
        }
    }

    if (not pgrs_.empty()) {
        for (auto &&prog : pgrs_) {
            prog->gen_asm(fs);
            fs << std::endl;
        }
    }

    if (not lc_pool_.empty()) {
        fs << "\t.section\t.rodata" << std::endl;
        fs << std::endl;
        for (auto &&loc : lc_pool_) {
            auto label = gen_loc_cst_label(loc);

            fs << "\t.global\t" << label << std::endl;
            fs << "\t.align\t" << 4 << std::endl;
            fs << "\t.type\t" << label << ", @object" << std::endl;
            fs << "\t.size\t" << label << ", " << 4 << std::endl;
            fs << label << ":" << std::endl;
            fs << "\t.word\t" << loc << std::endl;
            fs << std::endl;
        }
    }

    // compiler tag
    fs << "\t.ident\t\"c2rv: 0.1\"" << std::endl;
}