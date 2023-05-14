#include <codegen.hh>
#include <utils.hh>

CodeGen::CodeGen(const char *path) : bbs() {
  fs.open(path, std::ios::out);
  Assert(fs, "I/O Error");
}

void CodeGen::Generate() {
  BBVisitor visitor(this);
  for (auto &&bb: bbs) {
    bb->GenASM(&visitor);
  }
  fs.flush();
}

void CodeGen::PushBB(std::shared_ptr<ASMBasic> &bb) {
  bbs.push_back(std::move(bb));
}

CodeGen::~CodeGen() {
  fs.close();
}

BBVisitor::BBVisitor(CodeGen *_cg): cg(_cg) {}

void BBVisitor::toASM(GlobalValue *gv) {
  [[maybe_unused]] auto aligned_length = ROUNDUP(gv->len);
  cg->fs << gv->name << ":" << std::endl;
  // TODO: For array and g values


  cg->fs << std::endl;
}

void BBVisitor::toASM(GAttributes *ga) {
  cg->fs << ga->attr << std::endl;  
}

void BBVisitor::toASM(Progress *prog) {
  // TODO: translate to asm codes
  cg->fs << prog->name << ":" << std::endl;
  for (auto &&abb: prog->abbs) {
    cg->fs << abb->tagname << ":" << std::endl;
    for (auto &&inst: abb->asms) {
      cg->fs << inst->toString() << inst->Comment() << std::endl;
    }
  }
  cg->fs << std::endl;
}
