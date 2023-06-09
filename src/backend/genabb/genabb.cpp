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
                    uint64_t uint64_value = reinterpret_cast<uint64_t &>(double_value);
                    result.push_back(uint64_value);
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
    gval->name = value.first;
    auto &&gvp = std::dynamic_pointer_cast<GlobalValue>(value.second);
    Assert(gvp, "bad dynamic cast");

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
                    uint64_t uint64_value = reinterpret_cast<uint64_t &>(double_value);
                    gval->values.push_back(uint64_value);
                } else {
                    gval->values.push_back(static_cast<int64_t>(arg));
                }
            },
            val_ptr->GetValue());
    }
    return gval;
}

static std::unique_ptr<ABBProg> make_abbprog(NormalFuncPtr &func, NameValueMap &gvalues) {
    auto abbprog = std::make_unique<ABBProg>();
    abbprog->name = func->GetFuncName();
    auto &&plan = linear_plan(func);
    auto &&topo = func->TopoSortFromEntry();

    return abbprog;

    for (auto &&cfgnptr : topo) {
        abbprog->abbs.push_back(make_asm(cfgnptr, plan, func));
    }

    return abbprog;
}

void CodeGen::GenABB() {
    auto &&gvalues = comp_unit.getGlbTable().GetNameValueMap();
    reg_nvmap(gvalues);

    for (auto &&value : gvalues) {
        bbs.push_back(make_abbgval(value));
    }

    auto &&funcs = comp_unit.GetNormalFuncTable();
    for (auto &&func : funcs) {
        bbs.push_back(make_abbprog(func, gvalues));
    }

    auto info = std::make_unique<ABBGAttr>();
    info->attr +=
        "\t"
        ".ident"
        "\t"
        "\"c2rv: 1.0\""
        "\n";
    bbs.push_back(std::move(info));
}