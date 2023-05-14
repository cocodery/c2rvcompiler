#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <queue>
#include <set>
#include <vector>
#include <array>

#include <codegen.hh>
#include <riscv-asm.hh>

struct Point {
  size_t idx;
  size_t nxt;
  std::list<size_t> pre;
  std::shared_ptr<RVInst> inst;

  Point(size_t _idx, std::shared_ptr<RVInst> _inst) : idx(_idx), nxt(0), pre(), inst(_inst) {}
};

struct RegAppoint {
  size_t RApp = 0;
  size_t WApp = 0;
};

static size_t MEMWApp = 0;
static size_t MEMRApp = 0;

struct Overhead {
  size_t cost;
  size_t idx;

  Overhead(size_t cst, size_t i) : cost(cst), idx(i) {}

  bool operator<(const Overhead &rhs) const {
    if (this->cost > rhs.cost) {
      return false;
    } else if (this->cost == rhs.cost) {
      return this->idx > rhs.idx;
    }
    return true;
  }
};

static void SiFiveU740(std::shared_ptr<ASMBasicBlock> &abb);

template <typename T> struct deduce { typedef T type; };

template <typename T>
size_t diffsetsiz(std::vector<T> &lhs,
                  std::set<typename deduce<T>::type> &rhs) {
  std::vector<typename deduce<T>::type> recv;
  std::set_difference(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                      std::inserter(recv, recv.begin()));
  return recv.size();
}

void CodeGen::Issuer() {
  BBVisitor visitor(this);
  for (auto &&bb : bbs) {
    bb->Issue(&visitor);
  }
}

void BBVisitor::Issue(GlobalValue *gv) {}
void BBVisitor::Issue(GAttributes *ga) {}

void BBVisitor::Issue(Progress *prog) {
  for (auto &&abb : prog->abbs) {
    SiFiveU740(abb);
  }
}

static void GenAppListInfo(Point &pt, std::array<RegAppoint, 64> &arr) {
  if (auto &&rd = pt.inst->dst; rd != 0) {
    arr[rd].WApp = pt.idx;
  }
  std::set<rid_t> uniq{pt.inst->src1, pt.inst->src2, pt.inst->src3};
  for (auto &&elem : uniq) {
    arr[elem].RApp = pt.idx;
  }
  if (pt.inst->opkind == opKind::MEMW) {
    MEMWApp = pt.idx;
  } else if (pt.inst->opkind == opKind::MEMR) {
    MEMRApp = pt.idx;
  }
}

static void GenAppPointInfo(Point &pt, std::array<RegAppoint, 64> &arr,
                            std::vector<Point> &aov) {
  if (auto &&rd = pt.inst->dst; rd != 0) {
    // all write need pre w/r finished
    pt.pre.push_back(arr[rd].RApp);
    aov[arr[rd].RApp].nxt += 1;

    pt.pre.push_back(arr[rd].WApp);
    aov[arr[rd].WApp].nxt += 1;
  }
  std::set<rid_t> uniq{pt.inst->src1, pt.inst->src2, pt.inst->src3};
  for (auto &&elem : uniq) {
    // all read need pre w finished
    auto &&widx = arr[elem].WApp;
    pt.pre.push_back(widx);
    aov[widx].nxt += 1;
  }
  if (pt.inst->opkind == opKind::MEMW) {
    // all mem w/r should be done before a w
    pt.pre.push_back(MEMWApp);
    pt.pre.push_back(MEMRApp);
  } else if (pt.inst->opkind == opKind::MEMR) {
    // all mem w should be done before a r
    pt.pre.push_back(MEMWApp);
  }
}

static void GenAOVInfo(std::vector<Point> &aov) {
  std::array<RegAppoint, 64> applist;
  const auto &&len = aov.size();
  if (!len)
    return;
  MEMWApp = 0;
  MEMRApp = 0;
  GenAppListInfo(aov[1], applist);
  for (size_t i = 2; i < len; ++i) {
    GenAppPointInfo(aov[i], applist, aov);
    GenAppListInfo(aov[i], applist);
  }
  for (auto &&p : aov) {
    std::cout << p.idx << std::setw(4) << ": " << p.nxt << std::endl;
    for (auto &&idx : p.pre) {
      std::cout << std::setw(4) << idx;
    }
    std::cout << std::endl;
  }
}

static void Reorder(std::vector<Point> &aov,
                    std::list<std::shared_ptr<ASMInst>> &lst) {
  auto &&iter = lst.begin();
  const size_t len = aov.size();
  size_t idx = 1;

  std::set<size_t> issued{0};
  std::priority_queue<Overhead> ovs;

  for (size_t i = 1; i < len; ++i) {
    auto &&p = aov[i];
    if (p.pre.empty()) {
      ovs.emplace(p.nxt + p.inst->latency, p.idx);
    } else {
      p.pre.remove(0);
    }
  }

  while (idx != len) {
    if (ovs.empty())
      panic("schedule failed");
    
    idx += 1;
    auto tp = ovs.top();
    auto &&tgt = aov[tp.idx];
    ovs.pop();
    *iter = tgt.inst;
    iter ++;

    for (size_t i = 1; i < len; ++i) {
      auto &&p = aov[i];
      if (!p.pre.empty()) {
        p.pre.remove(tgt.idx);
        if (p.pre.empty()) {
          ovs.emplace(p.nxt + p.inst->latency, p.idx);
        }
      }
    }
  }
}

static void SiFiveU740(std::shared_ptr<ASMBasicBlock> &abb) {
  Assert(std::dynamic_pointer_cast<RVBasicBlock>(abb), "input type error");

  if (abb->asms.empty())
    return;
  std::vector<Point> aov;
  size_t idx = 0;
  aov.push_back(Point(idx++, nullptr));
  for (auto &&inst : abb->asms) {
    auto rvinst = std::dynamic_pointer_cast<RVInst>(inst);
    if (rvinst->opkind == opKind::BJ)
      break;
    aov.push_back(Point(idx++, rvinst));
  }
  GenAOVInfo(aov);
  Reorder(aov, abb->asms);
}
