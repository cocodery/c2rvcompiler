#pragma once

#include <string>
#include <sstream>
#include <cstdint>
#include <string_view>
#include <list>
#include <memory>

// MDR -> mul/div/rem
// BJ -> branch/jump

class ASMInst {
public:
  ASMInst() = default;
  virtual ~ASMInst() = default;
  virtual std::string_view toString();
  virtual std::string_view Comment();
  virtual void setComment(const char *_comt);
};

class ASMBasicBlock {
public:
  ASMBasicBlock() = default;
  virtual ~ASMBasicBlock() = default;

public:
  std::string tagname;
  std::list<std::shared_ptr<ASMInst>> asms;
};