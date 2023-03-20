#pragma once

#include <iostream>
#include <sstream>
#include <stdint.h>
#include <cassert>
#include <string>

using std::cout;
using std::endl;

using TypeID = uint64_t;

constexpr TypeID NONE       = (0ul << 0x0000);

constexpr TypeID BOOL       = (0ul << 0x0001);
constexpr TypeID INT        = (1ul << 0x0002);
constexpr TypeID FLOAT      = (1ul << 0x0003);
constexpr TypeID VOID       = (1ul << 0x0004);

constexpr TypeID CONST      = (1ul << 0x0005);
constexpr TypeID VARIABLE   = (1ul << 0x0006);

constexpr TypeID PARAM      = (1ul << 0x0007);

constexpr TypeID ARRAY      = (1ul << 0x0008);
constexpr TypeID POINTER    = (1ul << 0x0009);

constexpr TypeID GLOBAL     = (1ul << 0x000A);

class BaseType {
private:
    TypeID tid;
public:
    BaseType(TypeID tid = NONE);
    ~BaseType() = default;

    TypeID getType() const;
    void resetType(TypeID);
    void checkType(TypeID) const;

    bool BoolType()     const;
    bool IntType()      const;
    bool FloatType()    const;
    bool VoidType()     const;

    bool ConstType()    const;
    bool VarType()      const;

    bool ParamType()    const;

    bool ArrayType()    const;
    bool PoniterType()  const;

    bool GlobalType()   const;

    virtual std::string toString() = 0;
    virtual std::string tollvmIR() = 0;
};

TypeID getTypeID(std::string);
