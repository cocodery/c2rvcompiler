#include "3tle3wa/backend/scheduler/FastUse.hh"

#include <algorithm>

#include "3tle3wa/backend/scheduler/SchedItem.hh"

static const std::unordered_map<SCHED_TYPE, size_t> prior_map{
    // 0 -
    {SCHED_TYPE::JMPBR, 0},
    {SCHED_TYPE::FENCE, 1},
    {SCHED_TYPE::FUNCCALL, 2},

    // 10 -
    {SCHED_TYPE::IMISC, 10},
    {SCHED_TYPE::IMUL, 11},
    {SCHED_TYPE::FMISC, 12},
    {SCHED_TYPE::FMUL, 13},
    {SCHED_TYPE::IDIVREM, 14},
    {SCHED_TYPE::FDIVREM, 15},

    // 50 -
    {SCHED_TYPE::STORE, 50},

    // 100 -
    {SCHED_TYPE::LOAD, 100},
};

static const std::unordered_map<SCHED_TYPE, size_t> overhead_map{
    // assuming 0
    {SCHED_TYPE::JMPBR, 0},
    {SCHED_TYPE::FENCE, 0},
    {SCHED_TYPE::FUNCCALL, 0},

    // misc
    {SCHED_TYPE::STORE, 2},
    {SCHED_TYPE::IMISC, 2},
    {SCHED_TYPE::IMUL, 3},
    {SCHED_TYPE::FMISC, 6},
    {SCHED_TYPE::FMUL, 9},
    {SCHED_TYPE::IDIVREM, 30},
    {SCHED_TYPE::FDIVREM, 45},
    {SCHED_TYPE::LOAD, 5},
};

bool SchedFastUse::Wrapper::operator<(const Wrapper &other) const {
    auto self_weight = prior_map.at(node->self->type);
    auto other_weight = prior_map.at(other.node->self->type);

    if (self_weight == other_weight) {
        return node->post.size() < other.node->post.size();
    }

    return self_weight < other_weight;
}

bool SchedFastUse::Reservation::operator<(const Reservation &other) const { return avail_time > other.avail_time; }

SchedFastUse::SchedFastUse() : fencer_(nullptr), call_(nullptr), status_(), memory_(), am_(), storage_(), aovfree_() {
    am_.busy.emplace(SCHED_TYPE::JMPBR, false);
    am_.busy.emplace(SCHED_TYPE::FENCE, false);
    am_.busy.emplace(SCHED_TYPE::FUNCCALL, false);
    am_.busy.emplace(SCHED_TYPE::STORE, false);

    am_.busy.emplace(SCHED_TYPE::IMISC, false);
    am_.busy.emplace(SCHED_TYPE::IMUL, false);
    am_.busy.emplace(SCHED_TYPE::FMISC, false);
    am_.busy.emplace(SCHED_TYPE::FMUL, false);
    am_.busy.emplace(SCHED_TYPE::IDIVREM, false);
    am_.busy.emplace(SCHED_TYPE::FDIVREM, false);
    am_.busy.emplace(SCHED_TYPE::LOAD, false);
}

void SchedFastUse::Push(SchedItem *item) {
    auto node = std::make_unique<AOVNode>();
    node->issued = false;
    node->self = item;

    if (fencer_ != nullptr) {
        node->prev.insert(fencer_);
        fencer_->post.insert(node.get());
    }

    switch (item->type) {
        case SCHED_TYPE::FUNCCALL: /* {
            if (call_ != nullptr) {
                node->prev.insert(call_);
                call_->post.insert(node.get());
            }
            call_ = node.get();
        } break; */

        case SCHED_TYPE::FENCE:
        case SCHED_TYPE::JMPBR: {
            fencer_ = node.get();

            for (auto &&[idx, stat] : status_) {
                auto &&[w, r] = stat;

                if (w != nullptr) {
                    fencer_->prev.insert(w);
                    w->post.insert(fencer_);
                }

                if (not r.empty()) {
                    fencer_->prev.insert(r.begin(), r.end());
                    std::for_each(r.begin(), r.end(), [this](AOVNode *node) -> void { node->post.insert(fencer_); });
                }
            }

            auto &&[mw, mr] = memory_;

            if (mw != nullptr) {
                fencer_->prev.insert(mw);
                mw->post.insert(fencer_);
            }

            if (not mr.empty()) {
                fencer_->prev.insert(mr.begin(), mr.end());
                std::for_each(mr.begin(), mr.end(), [this](AOVNode *node) -> void { node->post.insert(fencer_); });
            }
        } break;

        case SCHED_TYPE::STORE: {
            if (not memory_.reader.empty()) {
                node->prev.insert(memory_.reader.begin(), memory_.reader.end());
                auto bptr = node.get();
                std::for_each(memory_.reader.begin(), memory_.reader.end(),
                              [bptr](AOVNode *node) -> void { node->post.insert(bptr); });
                memory_.reader.clear();
            }

            if (memory_.writer != nullptr) {
                node->prev.insert(memory_.writer);
                memory_.writer->post.insert(node.get());
            }

            memory_.writer = node.get();
        } break;

        case SCHED_TYPE::LOAD: {
            if (memory_.writer != nullptr) {
                node->prev.insert(memory_.writer);
                memory_.writer->post.insert(node.get());
            }

            memory_.reader.insert(node.get());
        } break;

        default:
            break;
    }

    auto &&rids = item->reads;
    for (auto &&vridx : rids) {
        if (auto fnd = status_.find(vridx); fnd == status_.end()) {
            status_.emplace(vridx, RWStatus{});
        }

        auto &&[w, r] = status_.at(vridx);

        if (w != nullptr) {
            node->prev.insert(w);
            w->post.insert(node.get());
        }

        r.insert(node.get());
    }

    auto &&wids = item->writes;
    if (not wids.empty()) {
        for (auto &&vridx : wids) {
            if (auto fnd = status_.find(vridx); fnd == status_.end()) {
                status_.emplace(vridx, RWStatus{});
            }

            auto &&[w, r] = status_.at(vridx);

            if (not r.empty()) {
                node->prev.insert(r.begin(), r.end());
                auto bptr = node.get();
                std::for_each(r.begin(), r.end(), [bptr](AOVNode *node) -> void { node->post.insert(bptr); });
                r.clear();
            }

            if (w != nullptr) {
                node->prev.insert(w);
                w->post.insert(node.get());
            }

            w = node.get();
        }
    }

    node->prev.erase(node.get());
    node->post.erase(node.get());

    if (node->prev.empty()) {
        aovfree_.push_back(Wrapper{.node = node.get()});
        std::push_heap(aovfree_.begin(), aovfree_.end());
    }

    storage_.push_back(std::move(node));
}

void SchedFastUse::Sched() {
    size_t now = 0;

    std::priority_queue<Reservation> rsvtbl;

    // issue the first instruction for initialzation
    if (not aovfree_.empty()) {
        auto one = aovfree_.front();

        std::pop_heap(aovfree_.begin(), aovfree_.end());
        aovfree_.pop_back();

        view_.push_back(one.node->self);
        one.node->issued = true;

        auto type = one.node->self->type;
        am_.busy.at(type) = true;

        auto resv = Reservation{.type = type, .avail_time = overhead_map.at(type) + now, .write = size_t(-1)};
        if (not one.node->self->writes.empty()) {
            resv.write = one.node->self->writes.at(0);
            am_.rdy[resv.write] = false;
        }
        rsvtbl.push(std::move(resv));

        for (auto &&post : one.node->post) {
            post->prev.erase(one.node);

            if (post->prev.empty() and not post->issued) {
                aovfree_.push_back(Wrapper{.node = post});
                std::push_heap(aovfree_.begin(), aovfree_.end());
            }
        }
    }

    auto cmp = [this](const Wrapper &lhs, const Wrapper &rhs) -> bool {
        for (auto &&v : lhs.node->self->reads) {
            if (am_.rdy.find(v) != am_.rdy.end() and not am_.rdy.at(v)) {
                return true;
            }
        }

        return lhs < rhs;
    };

    if (aovfree_.size() > 1) {
        std::make_heap(aovfree_.begin(), aovfree_.end(), cmp);
    }

    while (not aovfree_.empty()) {
        AOVNode *choice = nullptr;

        for (auto &&wrap : aovfree_) {
            auto node = wrap.node;
            auto type = node->self->type;

            bool can_issue = not am_.busy.at(type) and not node->issued;

            auto &&vec = node->self->reads;
            bool has_w = not node->self->writes.empty();
            if (has_w) {
                vec.push_back(node->self->writes.at(0));
            }

            for (auto &&v : vec) {
                auto rdy = true;
                if (auto fnd = am_.rdy.find(v); fnd != am_.rdy.end()) {
                    rdy = fnd->second;
                }

                can_issue = can_issue and rdy;
            }

            if (has_w) {
                vec.pop_back();
            }

            if (can_issue) {
                choice = wrap.node;
                break;
            }
        }

        if (choice == nullptr) {
            auto rsv = rsvtbl.top();
            rsvtbl.pop();

            now = rsv.avail_time;
            am_.busy.at(rsv.type) = false;
            if (rsv.write != size_t(-1)) {
                am_.rdy.at(rsv.write) = true;
            }
        } else {
            view_.push_back(choice->self);
            choice->issued = true;

            auto type = choice->self->type;
            am_.busy.at(type) = true;

            auto resv = Reservation{.type = type, .avail_time = overhead_map.at(type) + now, .write = size_t(-1)};
            if (not choice->self->writes.empty()) {
                resv.write = choice->self->writes.at(0);
                am_.rdy[resv.write] = false;
            }
            rsvtbl.push(std::move(resv));

            for (auto &&post : choice->post) {
                post->prev.erase(choice);

                if (post->prev.empty() and not post->issued) {
                    aovfree_.push_back(Wrapper{.node = post});
                    std::push_heap(aovfree_.begin(), aovfree_.end());
                }
            }

            now += 1;
        }

        while (not aovfree_.empty() and aovfree_.front().node->issued) {
            std::pop_heap(aovfree_.begin(), aovfree_.end(), cmp);
            aovfree_.pop_back();
        }

        while (not rsvtbl.empty() and rsvtbl.top().avail_time <= now) {
            auto &&rsv = rsvtbl.top();
            now = rsv.avail_time;
            am_.busy.at(rsv.type) = false;
            if (rsv.write != size_t(-1)) {
                am_.rdy.at(rsv.write) = true;
            }
            rsvtbl.pop();
        }
    }
}
