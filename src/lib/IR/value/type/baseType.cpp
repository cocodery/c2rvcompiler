#include "baseType.hh"

BaseType::BaseType(ATTR_TYPE _type, ATTR_MUTABLE _mutable, ATTR_POINTER _pointer, ATTR_SCALAR _scalar, ATTR_POSITION _position)
    : attr_type(_type), attr_mutable(_mutable), attr_pointer(_pointer), attr_scalar(_scalar), attr_position(_position) {
}

bool BaseType::voidType()   const { return (attr_type == VOID) ; }
bool BaseType::boolType()   const { return (attr_type == BOOL) ; }
bool BaseType::charType()   const { return (attr_type == CHAR) ; }
bool BaseType::intType()    const { return (attr_type == INT)  ; }
bool BaseType::floatType()  const { return (attr_type == FLOAT); }

bool BaseType::IsMutable()  const { return (attr_mutable == MUTABLE); }
bool BaseType::IsImMutable()const { return (attr_mutable == IMMUTABLE); }

bool BaseType::IsPointer()  const { return (attr_pointer == POINTER); }
bool BaseType::IsNotPtr()   const { return (attr_pointer == NOTPTR); }

bool BaseType::IsScalar()   const { return (attr_scalar == SCALAR); }
bool BaseType::isArray()    const { return (attr_scalar == ARRAY); }

bool BaseType::IsLocal()    const { return (attr_position == LOCAL); }
bool BaseType::IsParameter()const { return (attr_position == PARAMETER); }
bool BaseType::IsGlobal()   const { return (attr_position == GLOBAL); }

ATTR_TYPE BaseType::getAttrType() const {
    return attr_type;
}

void BaseType::resetAttrType(ATTR_TYPE _type) {
    attr_type = _type;
}

ATTR_MUTABLE BaseType::getAttrMutable() const {
    return attr_mutable;
}

ATTR_SCALAR BaseType::getAttrScalar() const {
    return attr_scalar;
}
