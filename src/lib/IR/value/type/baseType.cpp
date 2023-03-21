#include "baseType.hh"

BaseType::BaseType(TypeID _tid) : tid(_tid) {
    assert(this->tid & (BOOL | INT | FLOAT | VOID));
}

TypeID BaseType::getType() const {
    return this->tid;
}

void BaseType::resetType(TypeID _tid) {
    assert(this->tid & (BOOL | INT | FLOAT | VOID));
    tid = _tid;
}

void BaseType::checkType(TypeID _tid) const {
    assert(this->tid & _tid);
}

bool BaseType::BoolType()     const { return this->tid & BOOL; }
bool BaseType::IntType()      const { return this->tid & INT; }
bool BaseType::FloatType()    const { return this->tid & FLOAT; }
bool BaseType::VoidType()     const { return this->tid & VOID; }

bool BaseType::ConstType()    const { return this->tid & CONST; }
bool BaseType::VarType()      const { return this->tid & VARIABLE; }

bool BaseType::ParamType()    const { return this->tid & PARAM; }

bool BaseType::ArrayType()    const { return this->tid & ARRAY; }
bool BaseType::PoniterType()  const { return this->tid & POINTER; }

bool BaseType::GlobalType()   const { return this->tid & GLOBAL; }

TypeID getTypeID(std::string name) {
    TypeID tid =  (name == "int")   ? INT
                : (name == "float") ? FLOAT
                : (name == "void")  ? VOID
                : NONE;
    assert(tid != NONE);
    return tid;
}