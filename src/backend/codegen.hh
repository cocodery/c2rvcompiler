#pragma once

#include <fstream>
#include <list>
#include <memory>
#include <type_traits>
#include <cassert>

#include "asm.hh"

class CodeGen;

class BasicBlock;
class BBVisitor;

class GlobalValue;
class GAttributes;
class Progress;

class BasicBlock {
public:
  virtual void GenASM(BBVisitor *visitor) = 0;
  virtual ~BasicBlock() = default;
};

class BBVisitor {
  CodeGen *cg;

public:
  BBVisitor(CodeGen *_cg);
  void toASM(GlobalValue *gv);
  void toASM(GAttributes *ga);
  void toASM(Progress  *prog);
  ~BBVisitor() = default;
};

template <typename BBType> class VisitableBasicBlock : public BasicBlock {
  void GenASM(BBVisitor *visitor) {
    visitor->toASM(static_cast<BBType *>(this));
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
  std::list<std::shared_ptr<ASMInst>> asms;
};

class CodeGen {
  std::list<std::shared_ptr<BasicBlock>> bbs;
  std::fstream fs;
public:
  CodeGen(const char *path);
  ~CodeGen();

  void Generate();
  void push_bb(std::shared_ptr<BasicBlock> &bb);

  friend BBVisitor;
};