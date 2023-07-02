#pragma once

#include <cstddef>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "3tle3wa/ir/function/basefunc.hh"
#include "3tle3wa/ir/function/cfgNode.hh"
#include "3tle3wa/ir/value/baseValue.hh"
#include "3tle3wa/ir/value/type/listType.hh"
#include "3tle3wa/ir/value/type/scalarType.hh"

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

class LibraryFunction : public BaseFunction {
   public:
    LibraryFunction(ScalarTypePtr, std::string &, ParamList &);
    ~LibraryFunction() = default;

    bool IsLibFunction() const;

    virtual bool IsSYSYLibFunction() const;

    std::string tollvmIR();
};

class SYSYLibFunction;
using SYSYLibFuncPtr = std::shared_ptr<SYSYLibFunction>;

class SYSYLibFunction final : public LibraryFunction {
   public:
    SYSYLibFunction(ScalarTypePtr, std::string &, ParamList &);
    ~SYSYLibFunction() = default;

    bool IsSYSYLibFunction() const;

    static SYSYLibFuncPtr CreatePtr(ScalarTypePtr, std::string, ParamList &);
};

class LLVMLibFunction;
using LLVMLibFuncPtr = std::shared_ptr<LLVMLibFunction>;

class LLVMLibFunction final : public LibraryFunction {
   public:
    std::string proto_name;
    size_t proto_arg_nums;

    LLVMLibFunction(std::string &, size_t, ScalarTypePtr, std::string &, ParamList &);
    ~LLVMLibFunction() = default;

    bool IsSYSYLibFunction() const;

    std::string &GetProtoName();
    size_t GetProtoArgNums() const;

    static LLVMLibFuncPtr CreatePtr(std::string, size_t, ScalarTypePtr, std::string, ParamList &);
};

std::ostream &operator<<(std::ostream &, BaseFuncPtr);
