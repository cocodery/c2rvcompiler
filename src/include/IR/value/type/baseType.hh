#pragma once

#include <iostream>
#include <sstream>
#include <memory>
#include <cassert>
#include <string>
#include <algorithm>

using std::cout;
using std::endl;

using TypeID = uint64_t;

constexpr TypeID NONE       = (0ul << 0x0000);
// type of value
constexpr TypeID BOOL       = (1ul << 0x0001);
constexpr TypeID INT        = (1ul << 0x0002);
constexpr TypeID FLOAT      = (1ul << 0x0003);
constexpr TypeID VOID       = (1ul << 0x0004);
// type qualifier
constexpr TypeID CONST      = (1ul << 0x0005);
// contained-value attribute
constexpr TypeID CONSTANT   = (1ul << 0x0006);
constexpr TypeID VARIABLE   = (1ul << 0x0007);
constexpr TypeID PARAM      = (1ul << 0x0008);

constexpr TypeID ARRAY      = (1ul << 0x0009);
constexpr TypeID POINTER    = (1ul << 0x000A);
// global attribute
constexpr TypeID GLOBAL     = (1ul << 0x000B);

class BaseType {
private:
    TypeID tid;
    TypeID getType() const;
public:
    BaseType(TypeID tid = NONE);
    ~BaseType() = default;

    void resetType(TypeID);

    template<typename... TypeIDs>
    TypeID getMaskedType(TypeID first, TypeIDs... rest) const {
        TypeID res = getType() & first;
        for (auto tid : std::initializer_list<TypeID>{rest...}) {
            res |= getType() & tid;
        }
        return res;
    }

    template<typename... TypeIDs>
    bool checkType(TypeIDs... _tids) const {
        auto &&tids = {_tids...};
        return std::all_of(tids.begin(), tids.end(), [this](auto tid){ return __builtin_popcountll(getMaskedType(tid)) == 1; });
    }

    bool BoolType()     const;
    bool IntType()      const;
    bool FloatType()    const;
    bool VoidType()     const;

    bool ConstType()    const;

    bool ConstantType() const;
    bool VariableType() const;
    bool ParamType()    const;

    bool ArrayType()    const;
    bool PoniterType()  const;

    bool GlobalType()   const;

    virtual std::string toString() = 0;
    virtual std::string tollvmIR() = 0;
};

TypeID getTypeID(std::string);
