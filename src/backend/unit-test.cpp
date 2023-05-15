#include <codegen.hh>
#include <iostream>
#include <riscv-asm.hh>

int unittest() {
    auto x = std::make_shared<Progress>();
    std::shared_ptr<ASMBasic> bb = x;
    x->name = "test";

    auto abb = std::make_shared<RVBasicBlock>();
    abb->tagname = "begin";
    abb->asms.push_back(std::make_shared<RV_LW>(5, 2, 40));
    abb->asms.push_back(std::make_shared<RV_ADDW>(6, 5, 0));
    abb->asms.push_back(std::make_shared<RV_SW>(6, 2, 40));
    abb->asms.push_back(std::make_shared<RV_SUBW>(5, 6, 5));
    abb->asms.push_back(std::make_shared<RV_BEQZ$>(5, "begin"));
    x->abbs.push_back(abb);

    abb = std::make_shared<RVBasicBlock>();
    abb->tagname = "begin";
    abb->asms.push_back(std::make_shared<RV_LW>(5, 2, 40));
    abb->asms.push_back(std::make_shared<RV_ADDW>(6, 5, 0));
    abb->asms.push_back(std::make_shared<RV_SUBW>(5, 6, 5));
    abb->asms.push_back(std::make_shared<RV_SW>(6, 2, 40));
    abb->asms.push_back(std::make_shared<RV_BEQZ$>(5, "begin"));
    x->abbs.push_back(abb);

    CodeGen cg("./sysy.S");
    cg.PushBB(bb);
    cg.Issuer();
    cg.Generate();
    return 0;
}