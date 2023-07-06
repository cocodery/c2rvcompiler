#include <unordered_map>
#include <unordered_set>

#include "3tle3wa/backend/InternalTranslation.hh"
#include "3tle3wa/backend/asm/AsmGlobalValue.hh"
#include "3tle3wa/backend/rl/RLBasicBlock.hh"
#include "3tle3wa/backend/rl/RLGen.hh"
#include "3tle3wa/backend/rl/RLPlanner.hh"
#include "3tle3wa/backend/rl/RLProgress.hh"
#include "3tle3wa/backend/rl/RLUop.hh"
#include "3tle3wa/backend/rl/RLVirtualRegister.hh"
#include "3tle3wa/backend/utils.hh"
#include "3tle3wa/ir/IR.hh"
#include "3tle3wa/ir/instruction/opCode.hh"

void InternalTranslation::DoAssignment() {}