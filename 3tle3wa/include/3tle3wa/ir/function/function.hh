#pragma once

#include <functional>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <vector>

#include "3tle3wa/ir/function/basefunc.hh"
#include "3tle3wa/ir/function/cfgNode.hh"

class NormalFunction;
using NormalFuncPtr = std::shared_ptr<NormalFunction>;
using BlockList = std::list<CfgNodePtr>;

class NormalFunction final : public BaseFunction {
   private:
    CfgNodePtr entry;
    CfgNodePtr exit;

    size_t var_idx;
    size_t blk_idx;

   public:
    NormalFunction(ScalarTypePtr, std::string &, ParamList &);
    ~NormalFunction() = default;

    bool IsLibFunction() const;

    CfgNodePtr CreateEntry();
    CfgNodePtr CreateExit();
    CfgNodePtr CreateCfgNode(BlockAttr _attr = NORMAL);

    CfgNodePtr GetEntryNode();
    CfgNodePtr GetExitNode();

    void SetEntryNode(CfgNodePtr);
    void SetExitNode(CfgNodePtr);

    CfgNodeList TopoSortFromEntry();
    CfgNodeList TopoSortFromExit();

    void SetVarIdx(size_t);
    size_t GetVarIdx();

    void SetBlkIdx(size_t);
    size_t GetBlkIdx();

    static NormalFuncPtr CreatePtr(ScalarTypePtr, std::string &, ParamList &);

    std::string tollvmIR();
};

class LibraryFunction;
using LibFuncPtr = std::shared_ptr<LibraryFunction>;

class LibraryFunction final : public BaseFunction {
   public:
    LibraryFunction(ScalarTypePtr, std::string &, ParamList &);
    ~LibraryFunction() = default;

    bool IsLibFunction() const;

    static LibFuncPtr CreatePtr(ScalarTypePtr, std::string, ParamList &);

    std::string tollvmIR();
};

std::ostream &operator<<(std::ostream &, BaseFuncPtr);
