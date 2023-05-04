#pragma once

#include <map>
#include <queue>
#include <vector>

#include "basefunc.hh"
#include "cfgNode.hh"

class NormalFunction;
using NormalFuncPtr = std::shared_ptr<NormalFunction>;
using BlockList = std::list<CfgNodePtr>;

class NormalFunction : public BaseFunction {
   private:
    CfgNodePtr entry;
    CfgNodePtr exit;

    size_t var_idx;
    size_t blk_idx;

   public:
    NormalFunction(ScalarTypePtr, std::string &, ParamList &);
    ~NormalFunction() = default;

    CfgNodePtr CreateEntry();
    CfgNodePtr CreateExit();
    CfgNodePtr CreateCfgNode(BlockAttr _attr = NORMAL);

    CfgNodePtr GetEntryNode();
    CfgNodePtr GetExitNode();

    void SetEntryNode(CfgNodePtr);
    void SetExitNode(CfgNodePtr);

    CfgNodeList GetAllNodes();
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

class LibraryFunction : public BaseFunction {
   public:
    LibraryFunction(ScalarTypePtr, std::string &, ParamList &);
    ~LibraryFunction() = default;

    static LibFuncPtr CreatePtr(ScalarTypePtr, std::string, ParamList &);

    std::string tollvmIR();
};

std::ostream &operator<<(std::ostream &, BaseFuncPtr);
