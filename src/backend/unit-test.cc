#include <iostream>

#include "codegen.hh"
#include "riscv-asm.hh"

int main() {
  auto x = std::make_shared<Progress>();
  std::shared_ptr<BasicBlock> bb = x;
  x->name = "test";
  x->asms.push_back(std::make_shared<RV_ADDIW>(1, 2, 23));
  CodeGen cg("./sysy.S");
  cg.push_bb(bb);
  cg.Generate();
  return 0;
}