#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

#include "../../debug/Logs.hh"

class ASMInst {
   public:
    virtual std::string_view toString() = 0;
    virtual std::string_view Comment() = 0;
    virtual void setComment(const char *_comt) = 0;
};