#include "3tle3wa/riscv/cpu/SifiveU74.hh"

#include <algorithm>
#include <deque>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "3tle3wa/backend/asm/AsmBasicBlock.hh"
#include "3tle3wa/backend/asm/AsmInstruction.hh"
#include "3tle3wa/riscv/asm.hh"
#include "3tle3wa/riscv/spec.hh"

namespace riscv {

Transaction LA_LB::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction LLA_LB::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction LW_LB::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::LoadData,
                       .redundant_ = redundant};
}

Transaction LD_LB::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::LoadData,
                       .redundant_ = redundant};
}

Transaction SW_LB::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::StoreData,
                       .redundant_ = redundant};
}

Transaction SD_LB::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::StoreData,
                       .redundant_ = redundant};
}

Transaction FLW_LB::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::LoadData,
                       .redundant_ = redundant};
}

Transaction FSW_LB::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::StoreData,
                       .redundant_ = redundant};
}

Transaction MV::ToTrx() {
    bool redundant = (rd_ == rs_);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction LI::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction LUI::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SEXT_W::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SEQZ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SNEZ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SLTZ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SGTZ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SLT::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SLTI::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SLTIU::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SGT::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SLTU::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SGTU::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction FMV_S::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FABS_S::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FNEG_S::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction BEQZ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BNEZ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BLEZ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BGEZ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BLTZ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BGTZ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BEQ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BNE::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BLT::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BGE::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BGT::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BLE::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BLTU::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BGTU::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BLEU::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction BGEU::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction J::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction JAL::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction JR::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction JALR::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction RET::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction CALL::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction TAIL::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::JumpBranch,
                       .redundant_ = redundant};
}

Transaction LW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::LoadData,
                       .redundant_ = redundant};
}

Transaction LD::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::LoadData,
                       .redundant_ = redundant};
}

Transaction SW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::StoreData,
                       .redundant_ = redundant};
}

Transaction SD::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{},
                       .optype_ = OpType::StoreData,
                       .redundant_ = redundant};
}

Transaction ADDI::ToTrx() {
    bool redundant = (rd_ == rs_ and imm_ == 0);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction XORI::ToTrx() {
    bool redundant = (rd_ == rs_ and imm_ == 0);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction ORI::ToTrx() {
    bool redundant = (rd_ == rs_ and imm_ == 0);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction ANDI::ToTrx() {
    bool redundant = (rd_ == rs_ and imm_ == 0b111111111111);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SLLI::ToTrx() {
    bool redundant = (rd_ == rs_ and imm_ == 0);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SRLI::ToTrx() {
    bool redundant = (rd_ == rs_ and imm_ == 0);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SRAI::ToTrx() {
    bool redundant = (rd_ == rs_ and imm_ == 0);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction ADD::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SUB::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SLL::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction XOR::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SRL::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SRA::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction OR::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction AND::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction ADDIW::ToTrx() {
    bool redundant = (rd_ == rs_ and imm_ == 0);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SLLIW::ToTrx() {
    bool redundant = (rd_ == rs_ and imm_ == 0);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SRLIW::ToTrx() {
    bool redundant = (rd_ == rs_ and imm_ == 0);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SRAIW::ToTrx() {
    bool redundant = (rd_ == rs_ and imm_ == 0);
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction ADDW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SUBW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SLLW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SRLW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction SRAW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::IntegerOperation,
                       .redundant_ = redundant};
}

Transaction MUL::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::MulDivRemOperation,
                       .redundant_ = redundant};
}

Transaction DIV::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::MulDivRemOperation,
                       .redundant_ = redundant};
}

Transaction DIVU::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::MulDivRemOperation,
                       .redundant_ = redundant};
}

Transaction REM::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::MulDivRemOperation,
                       .redundant_ = redundant};
}

Transaction REMU::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::MulDivRemOperation,
                       .redundant_ = redundant};
}

Transaction MULW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::MulDivRemOperation,
                       .redundant_ = redundant};
}

Transaction DIVW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::MulDivRemOperation,
                       .redundant_ = redundant};
}

Transaction DIVWU::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::MulDivRemOperation,
                       .redundant_ = redundant};
}

Transaction REMW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::MulDivRemOperation,
                       .redundant_ = redundant};
}

Transaction REMWU::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::MulDivRemOperation,
                       .redundant_ = redundant};
}

Transaction FLW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{imm_},
                       .optype_ = OpType::LoadData,
                       .redundant_ = redundant};
}

Transaction FSW::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_{},
                       .other_info_{imm_},
                       .optype_ = OpType::StoreData,
                       .redundant_ = redundant};
}

Transaction FMADD_S::ToTrx() { return Transaction{}; }

Transaction FMSUB_S::ToTrx() { return Transaction{}; }

Transaction FNMADD_S::ToTrx() { return Transaction{}; }

Transaction FNMSUB_S::ToTrx() { return Transaction{}; }

Transaction FADD_S::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FSUB_S::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FMUL_S::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FDIV_S::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FCVT_W_S::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FCVT_S_W::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FEQ::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FLT::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FLE::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FMV_W_X::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FMV_X_W::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FMAX_S::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

Transaction FMIN_S::ToTrx() {
    bool redundant = false;
    return Transaction{.resource_required_{rs_, rt_},
                       .resource_occupied_ = rd_,
                       .other_info_{},
                       .optype_ = OpType::FloatRelated,
                       .redundant_ = redundant};
}

};  // namespace riscv

constexpr double latencyOf(riscv::OpType type) {
    switch (type) {
        case riscv::OpType::FloatRelated:
            return FLOAT_LATENCY;
        case riscv::OpType::IntegerOperation:
            return 1;
        case riscv::OpType::JumpBranch:
            return 2;
        case riscv::OpType::LoadData:
            return LOAD_HIT_LANTENCY;
        case riscv::OpType::StoreData:
            return 0;
        case riscv::OpType::MulDivRemOperation:
            return MUL_LATENCY * 0.7 + DIVREM_LATENCY * 0.3;
    }

    return 0;
}

// 特种操作一次只能发射一个
// 写后读和写后写会导致竞争，流水线停顿
// 有数据前递，大约一周期延迟

// 读写操作应该顺序进行，用于保证一致性
// 读操作可以扎堆进行，但是写操作必须顺序进行

class AbstractMachine {
    struct AOVNode {
        AsmInstruction *self_{nullptr};
        Transaction trx_{};

        std::unordered_set<AOVNode *> post_{};
        std::unordered_set<AOVNode *> prev_{};

        bool issued_{false};

        bool operator>(const AOVNode &other) {
            if (trx_.optype_ > other.trx_.optype_) {
                return true;
            } else if (trx_.optype_ < other.trx_.optype_) {
                return false;
            }

            if (post_.size() > other.post_.size()) {
                return true;
            } else {
                return false;
            }
        }

        bool operator==(const AOVNode &other) { return self_ == other.self_; }

        bool operator<(const AOVNode &other) {
            if (trx_.optype_ < other.trx_.optype_) {
                return true;
            } else if (trx_.optype_ > other.trx_.optype_) {
                return false;
            }

            if (post_.size() < other.post_.size()) {
                return true;
            } else {
                return false;
            }
        }
    };

    struct WRStatus {
        AOVNode *writer_{nullptr};
        std::vector<AOVNode *> reader_{};
    };

    WRStatus reg_[64] = {};
    WRStatus mem_{};
    AOVNode *fencer_{nullptr};

    std::list<std::unique_ptr<AOVNode>> storage_{};

    std::list<AsmInstruction *> view_{};

    std::set<AOVNode *> free_list_{};

   public:
    /**
     * 读写问题
     * 1. 对于一个单元，当仅有写者正在写入
     *       - 此时进入一个读者，读者的前驱为写者，并加入读者队列
     *       - 此时进入一个写者，写者的前驱为上一个写者，并更新写者为自己
     * 2. 对于一个单元，当仅有读者正在读取
     *       - 此时进入一个读者，加入读者队列
     *       - 此时进入一个写者，写者前驱为当前所有读者，更新写者为自己，并清空读者队列
     * 3. 对于一个单元，当有写者写入，也有读者读取
     *       - 保证读者队列的前驱为目前的写者
     *       - 此时进入一个读者，读者的前驱为写者，并加入读者队列
     *       - 此时进入一个写者，写者前驱为上一个写者和当前所有读者，更新写者为自己，并清空读者队列
     * 4. 对于一个 fence 属性指令，目前为跳转
     *       - 保证它是目前所有操作的后继，是后来所有操作的前驱
     */
    void Push(riscv::Instruction *inst, Transaction &&trx) {
        // ignore redundant code
        do /* ignore */ {
            if (trx.redundant_) {
                return;
            }
        } while (0);

        auto node = std::make_unique<AOVNode>();
        node->self_ = inst;
        node->trx_ = trx;

        if (fencer_ != nullptr) {
            node->prev_.insert(fencer_);
            fencer_->post_.insert(node.get());
        }

        if (node->trx_.optype_ == riscv::OpType::JumpBranch) {
            fencer_ = node.get();

            for (size_t i = 0; i < 64; ++i) {
                if (reg_[i].writer_ != nullptr) {
                    fencer_->prev_.insert(reg_[i].writer_);
                    reg_[i].writer_->post_.insert(fencer_);
                }

                if (not reg_[i].reader_.empty()) {
                    fencer_->prev_.insert(reg_[i].reader_.begin(), reg_[i].reader_.end());
                    std::for_each(reg_[i].reader_.begin(), reg_[i].reader_.end(),
                                  [this](AOVNode *node) -> void { node->post_.insert(fencer_); });
                }
            }
            if (mem_.writer_ != nullptr) {
                fencer_->prev_.insert(mem_.writer_);
                mem_.writer_->post_.insert(fencer_);
            }
            if (not mem_.reader_.empty()) {
                fencer_->prev_.insert(mem_.reader_.begin(), mem_.reader_.end());
                std::for_each(mem_.reader_.begin(), mem_.reader_.end(),
                              [this](AOVNode *node) -> void { node->post_.insert(fencer_); });
            }
        } else if (node->trx_.optype_ == riscv::OpType::LoadData) {
            if (mem_.writer_ != nullptr) {
                node->prev_.insert(mem_.writer_);
                mem_.writer_->post_.insert(node.get());

                mem_.reader_.push_back(node.get());
            } else {
                mem_.reader_.push_back(node.get());
            }
        } else if (node->trx_.optype_ == riscv::OpType::StoreData) {
            if (not mem_.reader_.empty()) {
                node->prev_.insert(mem_.reader_.begin(), mem_.reader_.end());
                auto bptr = node.get();
                std::for_each(mem_.reader_.begin(), mem_.reader_.end(),
                              [bptr](AOVNode *node) -> void { node->post_.insert(bptr); });
                mem_.reader_.clear();
            }
            if (mem_.writer_ != nullptr) {
                node->prev_.insert(mem_.writer_);
                mem_.writer_->post_.insert(node.get());
            }
            mem_.writer_ = node.get();
        }

        auto &&reqs = node->trx_.resource_required_;
        for (auto &&req : reqs) {
            if (req != 0 and req < 64) {
                if (reg_[req].writer_ != nullptr) {
                    node->prev_.insert(reg_[req].writer_);
                    reg_[req].writer_->post_.insert(node.get());

                    reg_[req].reader_.push_back(node.get());
                } else {
                    reg_[req].reader_.push_back(node.get());
                }
            }
        }

        auto &&occu = node->trx_.resource_occupied_;
        if (occu != 0 and occu < 64) {
            if (not reg_[occu].reader_.empty()) {
                node->prev_.insert(reg_[occu].reader_.begin(), reg_[occu].reader_.end());
                auto bptr = node.get();
                std::for_each(reg_[occu].reader_.begin(), reg_[occu].reader_.end(),
                              [bptr](AOVNode *node) -> void { node->post_.insert(bptr); });
                reg_[occu].reader_.clear();
            }
            if (reg_[occu].writer_ != nullptr) {
                node->prev_.insert(reg_[occu].writer_);
                reg_[occu].writer_->post_.insert(node.get());
            }
            reg_[occu].writer_ = node.get();
        }
        node->prev_.erase(node.get());
        node->post_.erase(node.get());

        if (node->prev_.empty()) {
            free_list_.insert(node.get());
        }

        storage_.push_back(std::move(node));
    }

    void Schedule() {
        AOVNode *prev_issued = nullptr;

        while (not free_list_.empty()) {
            if (free_list_.size() == 1) {
                auto one = (*free_list_.begin());
                free_list_.clear();

                view_.push_back(one->self_);
                one->issued_ = true;
                prev_issued = one;
                for (auto &&post : one->post_) {
                    post->prev_.erase(one);

                    if (post->prev_.empty() and not post->issued_) {
                        free_list_.insert(post);
                    }
                }

                continue;
            }

            auto it = free_list_.begin();

            AOVNode *good_to_issue = nullptr;

            AOVNode *choice = nullptr;

            while (it != free_list_.end()) {
                auto cur = *it;
                auto cur_type = cur->trx_.optype_;

                if (good_to_issue == nullptr) {
                    good_to_issue = cur;
                } else if (*cur > *good_to_issue) {
                    good_to_issue = cur;
                }

                if (prev_issued != nullptr) {
                    auto last_type = prev_issued->trx_.optype_;
                    auto last_write = prev_issued->trx_.resource_occupied_;

                    bool conflict = false;

                    for (auto &&v : cur->trx_.resource_required_) {
                        if (v == last_write) {
                            conflict = true;
                            break;
                        }
                    }

                    if (not conflict and (last_type == riscv::OpType::IntegerOperation or last_type != cur_type)) {
                        if (choice == nullptr) {
                            choice = cur;
                        } else if (*cur > *choice) {
                            choice = cur;
                        }
                    }
                }

                it++;
            }

            if (choice == nullptr) {
                choice = good_to_issue;
            }

            free_list_.erase(choice);

            prev_issued = choice;
            for (auto &&post : choice->post_) {
                post->prev_.erase(choice);

                if (post->prev_.empty() and not post->issued_) {
                    free_list_.insert(post);
                }
            }

            view_.push_back(choice->self_);
            choice->issued_ = true;
        }
    }

    std::list<AsmInstruction *> &Export() { return view_; }
};

void AsmBasicBlock::ArchSchedule() {
    AbstractMachine am;
    for (auto &&inst : insts_) {
        auto rvinst = dynamic_cast<riscv::Instruction *>(inst.get());
        am.Push(rvinst, rvinst->ToTrx());
    }

    am.Schedule();

    view_insts_.swap(am.Export());
}