#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

#include "../../debug/Logs.hh"

using rid_t = uint64_t;
using i32 = int32_t;
using cstr = const char *;

class ASMInst {
   public:
    virtual std::string_view toString() = 0;
    virtual std::string_view Comment() = 0;
    virtual void setComment(const char *_comt) = 0;
};