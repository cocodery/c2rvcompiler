#pragma once

#include <Logs.hh>
#include <cstdint>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
namespace backend {

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

}