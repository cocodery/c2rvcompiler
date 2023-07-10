#include "3tle3wa/backend/IntervalTree.hh"

#include "3tle3wa/utils/Logs.hh"

Interval::Interval(size_t len) : intval_(len / 64 + 1, 0) { len_ = len; }

size_t Interval::MaxLen() { return len_; }

void Interval::Cover(size_t begin, size_t end) {
    const auto &&begin_elem = begin / 64;
    const auto &&end_elem = end / 64;

    const auto &&begin_bit = begin - begin_elem * 64;
    const auto &&end_bit = end - end_elem * 64;

    for (size_t i = begin_elem + 1; i < end_elem; ++i) {
        intval_[i] = (size_t)-1;
    }

    const auto &&begin_msk = ~((1ul << begin_bit) - 1);
    const auto &&end_msk = (1ul << end_bit) - 1;

    if (begin_elem == end_elem) {
        intval_[begin_elem] |= (begin_msk & end_msk);
    } else {
        intval_[begin_elem] |= begin_msk;
        intval_[end_elem] |= end_msk;
    }
}

void Interval::Uncover(size_t begin, size_t end) {
    const auto &&begin_elem = begin / 64;
    const auto &&end_elem = end / 64;

    const auto &&begin_bit = begin - begin_elem * 64;
    const auto &&end_bit = end - end_elem * 64;

    for (size_t i = begin_elem + 1; i < end_elem; ++i) {
        intval_[i] = (size_t)0;
    }

    const auto &&begin_msk = ~((1ul << begin_bit) - 1);
    const auto &&end_msk = (1ul << end_bit) - 1;

    if (begin_elem == end_elem) {
        intval_[begin_elem] &= ~(begin_msk & end_msk);
    } else {
        intval_[begin_elem] &= ~begin_msk;
        intval_[end_elem] &= ~end_msk;
    }
}

bool Interval::operator[](size_t idx) {
    const auto &&elem = idx / 64;
    const auto &&bit = idx - elem * 64;

    const auto &&result = intval_[elem] & (1ul << bit);

    return result != 0;
}

void Interval::UpdateEnd(size_t begin, size_t end) {
    if ((*this)[end]) {
        return;
    }

    Cover(begin, end);
}

void Interval::UpdateBegin(size_t begin) { Uncover(0, begin); }

void Interval::Use() { use_ += 1; }

size_t Interval::UseTime() { return use_; }

size_t Interval::LiveSpan() {
    size_t span = 0;
    for (auto &&intval : intval_) {
        span += __builtin_popcountll(intval);
    }
    return span;
}

bool Interval::operator&&(Interval &other) {
    for (size_t i = 0; i < intval_.size(); ++i) {
        if (__builtin_popcountll(this->intval_[i] & other.intval_[i]) != 0) {
            return true;
        }
    }
    return false;
}

void Interval::operator|=(Interval &other) {
    for (size_t i = 0; i < intval_.size(); ++i) {
        this->intval_[i] |= other.intval_[i];
    }
}

void IntervalManager::New(size_t idx, size_t len) {
    if (auto fnd = intvals_.find(idx); fnd != intvals_.end()) {
        return;
    }

    auto intval = std::make_unique<Interval>(len);
    CRVC_UNUSE auto result = intvals_.emplace(idx, intval.get());
    Assert(result.second, "failed");

    storage_.push_back(std::move(intval));
}

Interval *IntervalManager::AskInterval(size_t idx) {
    if (auto fnd = intvals_.find(idx); fnd != intvals_.end()) {
        return fnd->second;
    }
    return nullptr;
}

void IntervalManager::UpdateBegin(size_t idx, size_t begin) {
    if (auto fnd = intvals_.find(idx); fnd != intvals_.end()) {
        fnd->second->UpdateBegin(begin);
    }
}

void IntervalManager::UpdateEnd(size_t idx, size_t begin, size_t end) {
    if (auto fnd = intvals_.find(idx); fnd != intvals_.end()) {
        fnd->second->UpdateEnd(begin, end);
    }
}

size_t IntervalManager::Num() { return storage_.size(); }

void IntervalManager::Hit(size_t lbidx) { return intvals_.at(lbidx)->Use(); }

void IntervalManager::CallAt(size_t lbidx, size_t pos) { (void)lbidx, (void)pos; }

size_t IntervalManager::BlkHit(size_t lbidx) { return intvals_.at(lbidx)->UseTime(); }

size_t IntervalManager::TotalHit() {
    size_t cnt = 0;
    for (auto &&intval : storage_) {
        cnt += intval->UseTime();
    }
    return cnt;
}

size_t IntervalManager::BlkLiveSpan(size_t lbidx) { return intvals_.at(lbidx)->LiveSpan(); }

size_t IntervalManager::TotalLiveSpan() {
    size_t cnt = 0;
    for (auto &&intval : storage_) {
        cnt += intval->LiveSpan();
    }
    return cnt;
}

bool IntervalManager::operator&&(IntervalManager &other) {
    for (auto &&[lbidx, intval] : intvals_) {
        auto ointval = other.AskInterval(lbidx);
        if (ointval != nullptr && (*ointval && *intval)) {
            return true;
        }
    }
    return false;
}

void IntervalManager::operator|=(IntervalManager &other) {
    for (auto &&[lbidx, ointval] : other.intvals_) {
        auto intval = AskInterval(lbidx);
        if (intval != nullptr) {
            *intval |= *ointval;
        } else {
            New(lbidx, ointval->MaxLen());
            (*AskInterval(lbidx)) |= *ointval;
        }
    }
}