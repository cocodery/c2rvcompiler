#include "../spec/riscv/riscv-asm.hh"
#include "genabb.hh"

using ilst = std::list<std::shared_ptr<ASMInst>>;
using transformer = void (*)(const InstPtr &, const allocplan &, ilst &);

static std::string cur_lable_prefix;
static NameValueMap *nvmap;

std::string gen_label(size_t idx) { return cur_lable_prefix + std::to_string(idx); }

uint64_t extract_imm(ConstantPtr &&cst) {
    Assert(cst->GetBaseType()->IsScalar() && cst->IsConstant(), "type error");
    uint64_t imm = true;
    std::visit(
        [&imm](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, float>) {
                double double_value = arg;
                imm = reinterpret_cast<uint64_t &>(double_value);
            } else {
                imm = static_cast<int64_t>(arg);
            }
        },
        cst->GetValue());
    return imm;
}

void De_None(const InstPtr &inst, const allocplan &plan, ilst &lst) {
    auto asminst = std::make_unique<RV_NOP>();
    lst.push_back(std::move(asminst));
}

void De_Jump(const InstPtr &inst, const allocplan &plan, ilst &lst) {
    auto llinst = std::dynamic_pointer_cast<JumpInst>(inst);
    Assert(llinst, "dynamic cast failed");
    auto &&label = gen_label(llinst->GetTarget()->GetBlockIdx());
    auto asminst = std::make_unique<RV_J$>(label.c_str());
    lst.push_back(std::move(asminst));
}

void De_Branch(const InstPtr &inst, const allocplan &plan, ilst &lst) {
    auto llinst = std::dynamic_pointer_cast<BranchInst>(inst);
    Assert(llinst, "dynamic cast failed");
    auto &&falselabel = gen_label(llinst->GetFalseTarget()->GetBlockIdx());

    auto &&cond = llinst->GetCondition();
    if (cond->IsConstant()) {
        auto cstcond = std::dynamic_pointer_cast<Constant>(cond);
        bool flag = false;

        std::visit(
            [&flag](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, float>) {
                    double double_value = arg;
                    flag = double_value == 0;
                } else {
                    flag = static_cast<int64_t>(arg) == 0;
                }
            },
            cstcond->GetValue());

        if (flag) {
            auto asminst = std::make_unique<RV_NOP>();
            lst.push_back(std::move(asminst));
            return;
        }

        auto &&label = gen_label(llinst->GetFalseTarget()->GetBlockIdx());
        auto asminst = std::make_unique<RV_J$>(label.c_str());
        lst.push_back(std::move(asminst));
        return;
    }

    auto &&cond_idx = plan.at(llinst->GetCondition()->guidx());
    auto asminst = std::make_unique<RV_BEQZ$>(cond_idx, falselabel.c_str());
    lst.push_back(std::move(asminst));
}

void De_Alloca(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_Store(const InstPtr &inst, const allocplan &plan, ilst &lst) {
    auto llinst = std::dynamic_pointer_cast<StoreInst>(inst);
    Assert(llinst, "dynamic cast failed");
    auto &&base = llinst->GetStoreAddr();
    panic("not implement yet");
}

void De_Gep(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_Call(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_Phi(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_Load(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_BitCast(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_SiToFp(const InstPtr &inst, const allocplan &plan, ilst &lst) {
    auto llinst = std::dynamic_pointer_cast<SitoFpInst>(inst);
    Assert(llinst, "dynamic cast failed");
    auto &&res = llinst->GetResult();
    auto &&opd = llinst->GetOprand();

    auto res_idx = plan.at(res->guidx());
    auto opd_idx = plan.at(opd->guidx());
    auto asminst = std::make_unique<RV_FCVT_W_S>(res_idx, opd_idx);
    lst.push_back(std::move(asminst));
}

void De_FpToSi(const InstPtr &inst, const allocplan &plan, ilst &lst) {
    auto llinst = std::dynamic_pointer_cast<SitoFpInst>(inst);
    Assert(llinst, "dynamic cast failed");
    auto &&res = llinst->GetResult();
    auto &&opd = llinst->GetOprand();

    auto res_idx = plan.at(res->guidx());
    auto opd_idx = plan.at(opd->guidx());
    auto asminst = std::make_unique<RV_FCVT_S_W>(res_idx, opd_idx);
    lst.push_back(std::move(asminst));
}

void De_Zext(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_ADD(const InstPtr &inst, const allocplan &plan, ilst &lst) {
    panic("not implement yet");
    auto llinst = std::dynamic_pointer_cast<BinaryInstruction>(inst);
    Assert(llinst, "dynamic cast failed");
    auto &&res = llinst->GetResult();
    auto &&lhs = llinst->GetLHS();
    auto &&rhs = llinst->GetRHS();
}

void De_OP_SUB(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_MUL(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_DIV(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_REM(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_LSHIFT(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_RSHIFT(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_LTH(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_LEQ(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_GTH(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_GEQ(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_EQU(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_OP_NEQ(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

void De_Ret(const InstPtr &inst, const allocplan &plan, ilst &lst) { panic("not implement yet"); }

std::unordered_map<OpCode, transformer> opmap{
    {None, De_None},           {Ret, De_Ret},       {Jump, De_Jump},     {Branch, De_Branch}, {Alloca, De_Alloca},
    {Store, De_Store},         {Gep, De_Gep},       {Call, De_Call},     {Phi, De_Phi},       {Load, De_Load},
    {BitCast, De_BitCast},     {SiToFp, De_SiToFp}, {FpToSi, De_FpToSi}, {Zext, De_Zext},     {OP_ADD, De_OP_ADD},
    {OP_SUB, De_OP_SUB},       {OP_MUL, De_OP_MUL}, {OP_DIV, De_OP_DIV}, {OP_REM, De_OP_REM}, {OP_LSHIFT, De_OP_LSHIFT},
    {OP_RSHIFT, De_OP_RSHIFT}, {OP_LTH, De_OP_LTH}, {OP_LEQ, De_OP_LEQ}, {OP_GTH, De_OP_GTH}, {OP_GEQ, De_OP_GEQ},
    {OP_EQU, De_OP_EQU},       {OP_NEQ, De_OP_NEQ},
};

void reg_nvmap(NameValueMap &gvalues) { nvmap = &gvalues; }

std::unique_ptr<ABBProg::ABBlock> make_asm(CfgNodePtr &cfgnptr, allocplan &plan, NormalFuncPtr &func) {
    cur_lable_prefix = std::string("L.") + func->GetFuncName() + ".";
    auto &&insts = cfgnptr->GetInstList();

    auto abblock = std::make_unique<ABBProg::ABBlock>();
    abblock->tagname = gen_label(cfgnptr->GetBlockIdx());

    for (auto &&inst : insts) {
        opmap.at(inst->GetOpCode())(inst, plan, abblock->asminsts);
    }

    return abblock;
}