#pragma once

#include <iostream>
#include <sstream>
#include <memory>
#include <cassert>
#include <string>
#include <algorithm>

using std::cout;
using std::endl;

enum ATTR_TYPE {
    VOID,
    BOOL,
    CHAR,
    INT,
    FLOAT
};

enum ATTR_MUTABLE {
    NONE1 = 5, // ignore attribute mutable 
    MUTABLE,
    IMMUTABLE
};

enum ATTR_POINTER {
    NONE2 = 8, // ignore attribute pointer
    POINTER,
    NOTPTR
};

enum ATTR_SCALAR {
    NONE3 = 11, // ignore attribute scalar
    SCALAR,
    ARRAY
};

enum ATTR_POSITION {
    NONE4 = 14, // ignore attribute position
    LOCAL,
    PARAMETER,
    GLOBAL
};

class BaseType;
using BaseTypePtr = std::shared_ptr<BaseType>;

class BaseType {
protected:
    ATTR_TYPE       attr_type; // void / bool / char / int / float
    ATTR_MUTABLE    attr_mutable; // const / non-const
    ATTR_POINTER    attr_pointer; // pointer / non-pointer
    ATTR_SCALAR     attr_scalar; // scalar / list
    ATTR_POSITION   attr_position; // local / parameter / global

public:
    BaseType(ATTR_TYPE, ATTR_MUTABLE, ATTR_POINTER, ATTR_SCALAR, ATTR_POSITION);
    BaseType(const BaseType &);
    ~BaseType() = default;

    bool voidType()     const;
    bool boolType()     const;
    bool charType()     const;
    bool intType()      const;
    bool floatType()    const;

    bool IsMutable()    const;
    bool IsImMutable()  const;

    bool IsPointer()    const;
    bool IsNotPtr()     const;

    bool IsScalar()     const;
    bool isArray()      const;

    bool IsLocal()      const;
    bool IsParameter()  const;
    bool IsGlobal()     const;

    void resetAttrType(ATTR_TYPE);

    virtual std::string tollvmIR() { assert(false); };
};
