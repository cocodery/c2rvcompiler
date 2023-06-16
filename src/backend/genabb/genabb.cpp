#include "genabb.hh"

static std::vector<uint32_t> declarr(ConstArrayPtr &bvaptr) {
    std::vector<uint32_t> result;
    auto &&cstarr = bvaptr->GetConstArr();
    for (auto &&v : cstarr) {
        std::visit(
            [&result](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, float>) {
                    double double_value = arg;
                    float float_value = double_value;
                    uint32_t uint32_value = reinterpret_cast<uint32_t &>(float_value);
                    result.push_back(uint32_value);
                } else {
                    result.push_back(static_cast<int64_t>(arg));
                }
            },
            v->GetValue());
    }
    return result;
}

static std::unique_ptr<ABBGValue> make_abbgval(std::pair<const std::string, BaseValuePtr> &value) {
    auto gval = std::make_unique<ABBGValue>();
    auto &&gvp = std::dynamic_pointer_cast<GlobalValue>(value.second);
    Assert(gvp, "bad dynamic cast");
    gval->name = "LG." + std::to_string(gvp->GetGlobalValueIdx());

    auto &&initval = gvp->GetInitValue();
    if (initval->IsUnInitVar()) {
        gval->isUninit = true;
        auto &&uninit_ptr = std::dynamic_pointer_cast<UnInitVar>(initval);
        if (uninit_ptr->GetBaseType()->IsArray()) {
            auto &&uninit_arrty_ptr = std::dynamic_pointer_cast<ListType>(initval->GetBaseType());
            gval->totalsiz = uninit_arrty_ptr->GetCapacity();
        } else {
            gval->totalsiz = 1;
        }
        return gval;
    }

    gval->isUninit = false;

    if (gvp->GetBaseType()->IsArray()) {
        auto &&arr_ptr = std::dynamic_pointer_cast<ConstArray>(gvp->GetInitValue());
        auto &&arrty_ptr = std::dynamic_pointer_cast<ListType>(arr_ptr->GetBaseType());
        gval->totalsiz = arrty_ptr->GetCapacity();
        gval->values = declarr(arr_ptr);
    } else {
        auto &&val_ptr = std::dynamic_pointer_cast<Constant>(gvp->GetInitValue());
        gval->totalsiz = 1;

        std::visit(
            [&gval](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, float>) {
                    double double_value = arg;
                    float float_value = double_value;
                    uint32_t uint32_value = reinterpret_cast<uint32_t &>(float_value);
                    gval->values.push_back(uint32_value);
                } else {
                    gval->values.push_back(static_cast<int64_t>(arg));
                }
            },
            val_ptr->GetValue());
    }
    return gval;
}

static std::unique_ptr<ABBProg> make_abbprog(NormalFuncPtr &func, NameValueMap &gvalues) {
    (void)gvalues;
    auto abbprog = std::make_unique<ABBProg>();
    FunctoProg(func.get(), abbprog.get());
    return abbprog;
}

static std::unique_ptr<ABBGAttr> make_abbgattr(std::string &str) {
    auto attr = std::make_unique<ABBGAttr>();
    attr->attr = std::move(str);
    return attr;
}

static std::unique_ptr<ABBGAttr> make_abbgattr(std::string &&str) { return make_abbgattr(str); }

void CodeGen::GenABB() {
    // no pic
    bbs.push_back(make_abbgattr("\t.option nopic"));

    // arch info
    bbs.push_back(make_abbgattr("\t.attribute arch, \"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0\""));

    // aligned access
    bbs.push_back(make_abbgattr("\t.attribute unaligned_access, 0"));

    // stack aligned
    bbs.push_back(make_abbgattr("\t.attribute stack_align, 16"));

    auto &&gvalues = comp_unit.getGlbTable().GetNameValueMap();

    // start gvalue decl
    if (!gvalues.empty()) {
        bbs.push_back(make_abbgattr("\t.data"));
    }

    for (auto &&value : gvalues) {
        if (value.second->IsGlobalValue()) bbs.push_back(make_abbgval(value));
    }

    auto &&funcs = comp_unit.GetNormalFuncTable();
    for (auto &&func : funcs) {
        bbs.push_back(make_abbprog(func, gvalues));
    }

    // compiler tag
    bbs.push_back(make_abbgattr("\t.ident\t\"c2rv: 1.0\""));
}