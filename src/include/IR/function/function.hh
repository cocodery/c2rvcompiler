#pragma once

#include <vector>

#include "../valueHeader.hh"
#include "basicblock.hh"

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

    ScalarTypePtr getReturnType();

    std::string &getFuncName();

    ParamList &getParamList();

    virtual std::string tollvmIR() = 0;
};

class NormalFunction;
using NormalFuncPtr = std::shared_ptr<NormalFunction>;
using BlockList     = std::list<BlockPtr>;

class NormalFunction : public BaseFunction {
private:
    BlockList block_list;
public:
    NormalFunction(ScalarTypePtr, std::string &, ParamList &);
    ~NormalFunction() = default;

    BlockPtr createBB();

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
