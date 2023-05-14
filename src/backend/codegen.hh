#pragma once

#include <fstream>
#include <list>
#include <memory>
#include <type_traits>

#include <asm.hh>
#include <Logs.hh>

class CodeGen;

class BasicBlock;
class BBVisitor;

class GlobalValue;
class GAttributes;
class Progress;

class ASMBasic {
public:
  virtual void GenASM(BBVisitor *visitor) = 0;
  virtual void Issue(BBVisitor *visitor) = 0;
  virtual ~ASMBasic() = default;
};

class BBVisitor {
  CodeGen *cg;

public:
  BBVisitor(CodeGen *_cg);
  void toASM(GlobalValue *gv);
  void toASM(GAttributes *ga);
  void toASM(Progress  *prog);

  void Issue(GlobalValue *gv);
  void Issue(GAttributes *ga);
  void Issue(Progress  *prog);
  ~BBVisitor() = default;
};

template <typename BType> class VisitableBasicBlock : public ASMBasic {
  void GenASM(BBVisitor *visitor) {
    visitor->toASM(static_cast<BType *>(this));
  }
  void Issue(BBVisitor *visitor) {
    visitor->Issue(static_cast<BType *>(this));
  }
};

class GlobalValue : public VisitableBasicBlock<GlobalValue> {
public:
  std::string name;
  size_t len;
};

class GAttributes : public VisitableBasicBlock<GAttributes> {
public:
  std::string attr;
};

class Progress : public VisitableBasicBlock<Progress> {
public:
  std::string name;
  std::list<std::shared_ptr<ASMBasicBlock>> abbs;
};

class CodeGen {
  std::list<std::shared_ptr<ASMBasic>> bbs;
  std::fstream fs;
public:
  CodeGen(const char *path);
  ~CodeGen();

  void Generate();
  void Issuer();
  void PushBB(std::shared_ptr<ASMBasic> &bb);

  friend BBVisitor;
};