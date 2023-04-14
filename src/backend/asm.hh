#pragma once

#include <string>
#include <sstream>
#include <cstdint>
#include <string_view>

class ASMInst {
public:
  ASMInst() = default;
  virtual ~ASMInst() = default;
  virtual std::string_view toString() = 0;
  virtual std::string_view Comment() = 0;
  virtual void setComment(const char *_comt) = 0;
};