#include "3tle3wa/ir/instHeader.hh"
#include "3tle3wa/backend/InternalTranslation.hh"

// control flow inst
void ReturnInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void JumpInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void BranchInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

// compare inst
void ICmpInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void FCmpInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

// binary inst
void IBinaryInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void FBinaryInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void FNegInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

// memory inst
void AllocaInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void StoreInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void LoadInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void GetElementPtrInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

// type convert
void SitoFpInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void FptoSiInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void ZextInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

// other inst
void CallInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void BitCastInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}

void PhiInst::TranslateTo(InternalTranslation &itx) {
    itx.Translate(this);
}