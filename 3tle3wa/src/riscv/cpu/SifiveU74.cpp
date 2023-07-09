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
    return Transaction{
        .resource_required_{}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction LLA_LB::ToTrx() {
    return Transaction{
        .resource_required_{}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction LW_LB::ToTrx() {
    return Transaction{.resource_required_{}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::LoadData}};
}

Transaction LD_LB::ToTrx() {
    return Transaction{.resource_required_{}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::LoadData}};
}

Transaction SW_LB::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::StoreData}};
}

Transaction SD_LB::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::StoreData}};
}

Transaction FLW_LB::ToTrx() {
    return Transaction{.resource_required_{rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::LoadData}};
}

Transaction FSW_LB::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::StoreData}};
}

Transaction MV::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction LI::ToTrx() {
    return Transaction{
        .resource_required_{}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction LUI::ToTrx() {
    return Transaction{
        .resource_required_{}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction SEXT_W::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SEQZ::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SNEZ::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SLTZ::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SGTZ::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SLT::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SLTI::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction SLTIU::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction SGT::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SLTU::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SGTU::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction FMV_S::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FABS_S::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FNEG_S::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction BEQZ::ToTrx() {
    return Transaction{.resource_required_{rs_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BNEZ::ToTrx() {
    return Transaction{.resource_required_{rs_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BLEZ::ToTrx() {
    return Transaction{.resource_required_{rs_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BGEZ::ToTrx() {
    return Transaction{.resource_required_{rs_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BLTZ::ToTrx() {
    return Transaction{.resource_required_{rs_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BGTZ::ToTrx() {
    return Transaction{.resource_required_{rs_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BEQ::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BNE::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BLT::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BGE::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BGT::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BLE::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BLTU::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BGTU::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BLEU::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction BGEU::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction J::ToTrx() {
    return Transaction{.resource_required_{}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction JAL::ToTrx() {
    return Transaction{.resource_required_{}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction JR::ToTrx() {
    return Transaction{.resource_required_{rs_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction JALR::ToTrx() {
    return Transaction{.resource_required_{rs_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction RET::ToTrx() {
    return Transaction{.resource_required_{}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction CALL::ToTrx() {
    return Transaction{.resource_required_{}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction TAIL::ToTrx() {
    return Transaction{.resource_required_{}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::JumpBranch}};
}

Transaction LW::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::LoadData}};
}

Transaction LD::ToTrx() {
    return Transaction{.resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::LoadData}};
}

Transaction SW::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::StoreData}};
}

Transaction SD::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{}, .optype_{OpType::StoreData}};
}

Transaction ADDI::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction XORI::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction ORI::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction ANDI::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction SLLI::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction SRLI::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction SRAI::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction ADD::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SUB::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SLL::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction XOR::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SRL::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SRA::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction OR::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction AND::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction ADDIW::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction SLLIW::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction SRLIW::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction SRAIW::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::IntegerOperation}};
}

Transaction ADDW::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SUBW::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SLLW::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SRLW::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction SRAW::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::IntegerOperation}};
}

Transaction MUL::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::MulDivRemOperation}};
}

Transaction DIV::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::MulDivRemOperation}};
}

Transaction DIVU::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::MulDivRemOperation}};
}

Transaction REM::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::MulDivRemOperation}};
}

Transaction REMU::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::MulDivRemOperation}};
}

Transaction MULW::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::MulDivRemOperation}};
}

Transaction DIVW::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::MulDivRemOperation}};
}

Transaction DIVWU::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::MulDivRemOperation}};
}

Transaction REMW::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::MulDivRemOperation}};
}

Transaction REMWU::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::MulDivRemOperation}};
}

Transaction FLW::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{imm_}, .optype_{OpType::LoadData}};
}

Transaction FSW::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{}, .other_info_{imm_}, .optype_{OpType::StoreData}};
}

Transaction FMADD_S::ToTrx() { return Transaction{}; }

Transaction FMSUB_S::ToTrx() { return Transaction{}; }

Transaction FNMADD_S::ToTrx() { return Transaction{}; }

Transaction FNMSUB_S::ToTrx() { return Transaction{}; }

Transaction FADD_S::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FSUB_S::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FMUL_S::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FDIV_S::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FCVT_W_S::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FCVT_S_W::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FEQ::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FLT::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FLE::ToTrx() {
    return Transaction{
        .resource_required_{rs_, rt_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FMV_W_X::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
}

Transaction FMV_X_W::ToTrx() {
    return Transaction{
        .resource_required_{rs_}, .resource_occupied_{rd_}, .other_info_{}, .optype_{OpType::FloatRelated}};
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
                    if (last_type == riscv::OpType::IntegerOperation or last_type != cur_type) {
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