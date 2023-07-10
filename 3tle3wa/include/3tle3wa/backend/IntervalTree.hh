#pragma once

#include <cstddef>
#include <cstring>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

class Interval {
    std::vector<size_t> intval_;

    size_t len_;

    size_t use_ = 0;

   public:
    Interval(size_t len);

    size_t MaxLen();

    // [begin, end)
    void Cover(size_t begin, size_t end);

    void Uncover(size_t begin, size_t end);

    void UpdateBegin(size_t begin);

    void UpdateEnd(size_t begin, size_t end);

    void Use();

    size_t UseTime();

    size_t LiveSpan();

    bool operator[](size_t idx);

    bool operator&&(Interval &other);

    void operator|=(Interval &other);
};

class IntervalManager {
    std::list<std::unique_ptr<Interval>> storage_;

    std::unordered_map<size_t, Interval *> intvals_;

   public:
    void New(size_t idx, size_t len);

    void UpdateBegin(size_t idx, size_t begin);

    void UpdateEnd(size_t idx, size_t begin, size_t end);

    Interval *AskInterval(size_t idx);

    size_t Num();

    void Hit(size_t lbidx);

    void CallAt(size_t lbidx, size_t pos);

    size_t BlkHit(size_t lbidx);
    size_t TotalHit();

    size_t BlkLiveSpan(size_t lbidx);
    size_t TotalLiveSpan();

    // void CheapSplits(IntervalManager &other);

    bool operator&&(IntervalManager &other);

    void operator|=(IntervalManager &other);
};