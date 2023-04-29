#pragma once

#include <queue>
#include <vector>

#include "cfgNode.hh"
#include "valueHeader.hh"

class BaseFunction;
using BaseFuncPtr = std::shared_ptr<BaseFunction>;
using ParamList = std::vector<BaseValuePtr>;

class BaseFunction {
   protected:
    ScalarTypePtr ret_type;
    std::string func_name;
    ParamList param_list;

   public:
    BaseFunction(ScalarTypePtr, std::string &, ParamList &);
    ~BaseFunction() = default;

    ScalarTypePtr GetReturnType();

    std::string &GetFuncName();

    ParamList &GetParamList();

    virtual std::string tollvmIR() = 0;
};

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
    CfgNodePtr CreateCfgNode();

    CfgNodePtr GetEntryNode();
    CfgNodePtr GetExitNode();
    CfgNodeList GetAllNodes();

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
