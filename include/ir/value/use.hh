#pragma once

#include <algorithm>
#include <list>
#include <memory>

class Use;
using UsePtr = std::shared_ptr<Use>;

class Instruction;
using InstPtr = std::shared_ptr<Instruction>;
using InstList = std::list<InstPtr>;

// As a Base Class of BaseValue
// record Instructions who use this Value
class Use {
   protected:
    InstList use_list;

   public:
    Use() = default;
    ~Use() = default;
};