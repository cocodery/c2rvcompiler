#pragma once

#include <string>

#include "ir/valueHeader.hh"

class BaseFunction;
using BaseFuncPtr = std::shared_ptr<BaseFunction>;

typedef std::set<BaseFuncPtr> CallerFunction;
typedef std::set<BaseFuncPtr> CalleeFunction;

class BaseFunction {
   protected:
    ScalarTypePtr ret_type;
    std::string func_name;
    ParamList param_list;

    bool side_effect;  // modify array or global-variable
    bool recursive;

    CallerFunction call_who;  // this call who
    CalleeFunction who_call;  // who call this

   public:
    BaseFunction(ScalarTypePtr, std::string &, ParamList &, bool _side_effect = false);
    ~BaseFunction() = default;

    virtual bool IsLibFunction() const = 0;

    bool GetSideEffect() const;
    void SetSideEffect(bool);

    bool GetRecursive() const;
    void SetRecursive(bool);

    void InsertCallWho(BaseFuncPtr);
    CallerFunction &GetCallWho();

    void InsertWhoCall(BaseFuncPtr);
    CalleeFunction &GetWhoCall();

    bool IsBeUsed() const;

    ScalarTypePtr GetReturnType();

    std::string &GetFuncName();

    ParamList &GetParamList();

    virtual std::string tollvmIR() = 0;
};