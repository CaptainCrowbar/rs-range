#pragma once

#include "rs-range/core.hpp"
#include "rs-tl/iterator.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

namespace RS::Range {

    // combinations

    struct CombinationsObject:
    AlgorithmBase<CombinationsObject> {
        size_t num;
        CombinationsObject(size_t k): num(k) {}
    };

    template <typename ForwardRange>
    class CombinationsIterator:
    public TL::InputIterator<CombinationsIterator<ForwardRange>,
        const Detail::SharedRange<TL::RangeValue<ForwardRange>>> {
    public:
        using range_iterator = TL::RangeIterator<const ForwardRange>;
        using value_type = TL::RangeValue<ForwardRange>;
        CombinationsIterator() = default;
        CombinationsIterator(range_iterator i, range_iterator j, size_t k):
            its_(Detail::make_shared_range<range_iterator>(i == j ? 0 : k, i)),
            val_(Detail::make_shared_range<value_type>(std::distance(its_.begin(), its_.end()), *i)),
            src_{i, j},
            count_(i == j ? npos : 0) {}
        const auto& operator*() const noexcept { return val_; }
        CombinationsIterator& operator++() {
            if (count_ == npos)
                return *this;
            auto& ivec = *its_.first.share;
            auto& vvec = *val_.first.share;
            size_t i = ivec.size();
            for (; i > 0; --i) {
                auto& j = ivec[i - 1];
                auto& v = vvec[i - 1];
                ++j;
                if (j != src_.second) {
                    v = *j;
                    break;
                }
                j = src_.first;
                v = *j;
            }
            if (i == 0)
                count_ = npos;
            else
                ++count_;
            return *this;
        }
        bool operator==(const CombinationsIterator& rhs) const noexcept { return count_ == rhs.count_; }
    private:
        Detail::SharedRange<range_iterator> its_;
        Detail::SharedRange<value_type> val_;
        TL::Irange<range_iterator> src_;
        size_t count_ = npos;
    };

    template <typename ForwardRange>
    TL::Irange<CombinationsIterator<ForwardRange>>
    operator>>(const ForwardRange& lhs, CombinationsObject rhs) {
        auto r = Detail::as_range(lhs);
        return {{r.begin(), r.end(), rhs.num}, {}};
    }

    inline CombinationsObject combinations(size_t k) { return {k}; }

    // flat_map

    template <typename UnaryFunction>
    struct FlatMapObject:
    AlgorithmBase<FlatMapObject<UnaryFunction>> {
        UnaryFunction fun;
        FlatMapObject(const UnaryFunction& f): fun(f) {}
    };

    template <typename Range, typename UnaryFunction>
    class FlatMapIterator:
    public TL::ForwardIterator<FlatMapIterator<Range, UnaryFunction>,
        const TL::RangeValue<std::invoke_result_t<UnaryFunction, TL::RangeValue<Range>>>> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using level_1_iterator = TL::RangeIterator<const Range>;
        using level_2_range = std::invoke_result_t<UnaryFunction, TL::RangeValue<Range>>;
        using level_2_iterator = TL::RangeIterator<level_2_range>;
        using function_type = std::function<level_2_range(const TL::RangeValue<Range>&)>;
        FlatMapIterator() = default;
        FlatMapIterator(level_1_iterator b, level_1_iterator e, UnaryFunction f):
            iter1_(b), end1_(e), range2_(), iter2_(), fun_(f) { update(); }
        const auto& operator*() const noexcept { return *iter2_; }
        FlatMapIterator& operator++() {
            ++iter2_;
            if (iter2_ == range2_->end()) {
                ++iter1_;
                range2_.reset();
                update();
            }
            return *this;
        }
        bool operator==(const FlatMapIterator& rhs) const noexcept {
            if (iter1_ != rhs.iter1_)
                return false;
            else if (range2_ && rhs.range2_)
                return iter2_ == rhs.iter2_;
            else
                return ! range2_ && ! rhs.range2_;
        }
    private:
        level_1_iterator iter1_, end1_;
        std::shared_ptr<level_2_range> range2_;
        level_2_iterator iter2_;
        function_type fun_;
        void update() {
            if (! range2_ && iter1_ != end1_) {
                range2_ = std::make_shared<level_2_range>(fun_(*iter1_));
                auto r = Detail::as_range(*range2_);
                iter2_ = r.begin();
            }
        }
    };

    template <typename Range, typename UnaryFunction>
    TL::Irange<FlatMapIterator<Range, UnaryFunction>> operator>>(const Range& lhs, FlatMapObject<UnaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.fun}, {e, e, rhs.fun}};
    }

    template <typename Container, typename UnaryFunction>
    Container& operator<<(Container& lhs, FlatMapObject<UnaryFunction> rhs) {
        Container temp;
        for (auto& x: lhs) {
            auto range = rhs.fun(x);
            auto r2 = Detail::as_range(range);
            std::copy(r2.begin(), r2.end(), append(temp));
        }
        lhs = std::move(temp);
        return lhs;
    }

    template <typename UnaryFunction>
    inline FlatMapObject<UnaryFunction> flat_map(UnaryFunction f) {
        return f;
    }

    // flatten

    struct FlattenObject:
    AlgorithmBase<FlattenObject> {};

    template <typename NestedRange>
    class FlattenIterator:
    public TL::ForwardIterator<FlattenIterator<NestedRange>,
        const TL::RangeValue<const TL::RangeValue<NestedRange>>> {
    public:
        using iterator_category = Detail::CommonCategory<NestedRange, std::forward_iterator_tag>;
        using level_1_iterator = TL::RangeIterator<const NestedRange>;
        using level_2_range = TL::RangeValue<NestedRange>;
        using level_2_iterator = TL::RangeIterator<const level_2_range>;
        FlattenIterator() = default;
        FlattenIterator(level_1_iterator b, level_1_iterator e):
            iter1_(b), end1_(e), iter2_(), end2_() { init2(); }
        const auto& operator*() const noexcept { return *iter2_; }
        FlattenIterator& operator++() {
            ++iter2_;
            if (iter2_ == end2_) {
                ++iter1_;
                init2();
            }
            return *this;
        }
        bool operator==(const FlattenIterator& rhs) const noexcept {
            if (iter1_ != rhs.iter1_)
                return false;
            else if (iter1_ == end1_ || rhs.iter1_ == rhs.end1_)
                return iter1_ == end1_ && rhs.iter1_ == rhs.end1_;
            else
                return iter2_ == rhs.iter2_;
        }
    private:
        level_1_iterator iter1_, end1_;
        level_2_iterator iter2_, end2_;
        void init2() {
            if (iter1_ == end1_) {
                iter2_ = end2_ = level_2_iterator();
            } else {
                auto r = Detail::as_range(*iter1_);
                iter2_ = r.begin();
                end2_ = r.end();
            }
        }
    };

    template <typename NestedRange>
    TL::Irange<FlattenIterator<NestedRange>> operator>>(const NestedRange& lhs, FlattenObject /*rhs*/) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e}, {e, e}};
    }

    constexpr FlattenObject flatten = {};

    // insert_before, insert_after, insert_between, insert_around

    template <typename T>
    struct InsertBeforeObject:
    AlgorithmBase<InsertBeforeObject<T>> {
        T value;
        InsertBeforeObject(const T& t): value(t) {}
    };

    template <typename T>
    struct InsertAfterObject:
    AlgorithmBase<InsertAfterObject<T>> {
        T value;
        InsertAfterObject(const T& t): value(t) {}
    };

    template <typename T>
    struct InsertBetweenObject:
    AlgorithmBase<InsertBetweenObject<T>> {
        T value;
        InsertBetweenObject(const T& t): value(t) {}
    };

    template <typename T>
    struct InsertAroundObject:
    AlgorithmBase<InsertAroundObject<T>> {
        T before, after;
        InsertAroundObject(const T& t1, const T& t2): before(t1), after(t2) {}
    };

    template <typename Range, typename T>
    class InsertBeforeIterator:
    public TL::ForwardIterator<InsertBeforeIterator<Range, T>, const T> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using range_iterator = TL::RangeIterator<const Range>;
        InsertBeforeIterator() = default;
        InsertBeforeIterator(range_iterator b, range_iterator e, const T& t):
            iter_(b), end_(e), value_(t), step_(0) {}
        const auto& operator*() const noexcept { return step_ == 0 ? value_ : *iter_; }
        InsertBeforeIterator& operator++() {
            if (step_ == 0) {
                step_ = 1;
            } else {
                ++iter_;
                step_ = 0;
            }
            return *this;
        }
        bool operator==(const InsertBeforeIterator& rhs) const noexcept {
            return iter_ == rhs.iter_ && step_ == rhs.step_;
        }
    private:
        range_iterator iter_, end_;
        T value_;
        uint8_t step_ = 0;
    };

    template <typename Range, typename T>
    class InsertAfterIterator:
    public TL::ForwardIterator<InsertAfterIterator<Range, T>, const T> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using range_iterator = TL::RangeIterator<const Range>;
        InsertAfterIterator() = default;
        InsertAfterIterator(range_iterator b, range_iterator e, const T& t):
            iter_(b), end_(e), value_(t), step_(0) {}
        const auto& operator*() const noexcept { return step_ == 0 ? *iter_ : value_; }
        InsertAfterIterator& operator++() {
            if (step_ == 0) {
                step_ = 1;
            } else {
                ++iter_;
                step_ = 0;
            }
            return *this;
        }
        bool operator==(const InsertAfterIterator& rhs) const noexcept {
            return iter_ == rhs.iter_ && step_ == rhs.step_;
        }
    private:
        range_iterator iter_, end_;
        T value_;
        uint8_t step_ = 0;
    };

    template <typename Range, typename T>
    class InsertBetweenIterator:
    public TL::ForwardIterator<InsertBetweenIterator<Range, T>, const T> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using range_iterator = TL::RangeIterator<const Range>;
        InsertBetweenIterator() = default;
        InsertBetweenIterator(range_iterator b, range_iterator e, const T& t):
            iter_(b), end_(e), value_(t), step_(b != e) {}
        const auto& operator*() const noexcept { return step_ == 0 ? value_ : *iter_; }
        InsertBetweenIterator& operator++() {
            if (step_ == 0) {
                step_ = 1;
            } else {
                ++iter_;
                step_ = 0;
            }
            return *this;
        }
        bool operator==(const InsertBetweenIterator& rhs) const noexcept {
            return iter_ == rhs.iter_ && step_ == rhs.step_;
        }
    private:
        range_iterator iter_, end_;
        T value_;
        uint8_t step_ = 0;
    };

    template <typename Range, typename T>
    class InsertAroundIterator:
    public TL::ForwardIterator<InsertAroundIterator<Range, T>, const T> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using range_iterator = TL::RangeIterator<const Range>;
        InsertAroundIterator() = default;
        InsertAroundIterator(range_iterator b, range_iterator e, const T& t1, const T& t2):
            iter_(b), end_(e), before_(t1), after_(t2), step_(0) {}
        const auto& operator*() const noexcept {
            switch (step_) {
                case 0:   return before_;
                case 1:   return *iter_;
                default:  return after_;
            }
        }
        InsertAroundIterator& operator++() {
            ++step_;
            if (step_ == 3) {
                step_ = 0;
                ++iter_;
            }
            return *this;
        }
        bool operator==(const InsertAroundIterator& rhs) const noexcept {
            return iter_ == rhs.iter_ && step_ == rhs.step_;
        }
    private:
        range_iterator iter_, end_;
        T before_, after_;
        uint8_t step_ = 0;
    };

    template <typename Range, typename T>
    TL::Irange<InsertBeforeIterator<Range, T>> operator>>(const Range& lhs, InsertBeforeObject<T> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.value}, {e, e, rhs.value}};
    }

    template <typename Range, typename T>
    TL::Irange<InsertAfterIterator<Range, T>> operator>>(const Range& lhs, InsertAfterObject<T> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.value}, {e, e, rhs.value}};
    }

    template <typename Range, typename T>
    TL::Irange<InsertBetweenIterator<Range, T>> operator>>(const Range& lhs, InsertBetweenObject<T> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.value}, {e, e, rhs.value}};
    }

    template <typename Range, typename T>
    TL::Irange<InsertAroundIterator<Range, T>> operator>>(const Range& lhs, InsertAroundObject<T> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.before, rhs.after}, {e, e, rhs.before, rhs.after}};
    }

    template <typename Container, typename T>
    Container& operator<<(Container& lhs, const InsertBeforeObject<T>& rhs) {
        Container temp;
        for (auto& x: lhs) {
            Detail::append_to(temp, rhs.value);
            Detail::append_to(temp, x);
        }
        lhs = std::move(temp);
        return lhs;
    }

    template <typename Container, typename T>
    Container& operator<<(Container& lhs, const InsertAfterObject<T>& rhs) {
        Container temp;
        for (auto& x: lhs) {
            Detail::append_to(temp, x);
            Detail::append_to(temp, rhs.value);
        }
        lhs = std::move(temp);
        return lhs;
    }

    template <typename Container, typename T>
    Container& operator<<(Container& lhs, const InsertBetweenObject<T>& rhs) {
        Container temp;
        auto i = lhs.begin(), e = lhs.end();
        if (i != e)
            Detail::append_to(temp, *i++);
        for (; i != e; ++i) {
            Detail::append_to(temp, rhs.value);
            Detail::append_to(temp, *i);
        }
        lhs = std::move(temp);
        return lhs;
    }

    template <typename Container, typename T>
    Container& operator<<(Container& lhs, const InsertAroundObject<T>& rhs) {
        Container temp;
        for (auto& x: lhs) {
            Detail::append_to(temp, rhs.before);
            Detail::append_to(temp, x);
            Detail::append_to(temp, rhs.after);
        }
        lhs = std::move(temp);
        return lhs;
    }

    template <typename T>
    inline InsertBeforeObject<T> insert_before(const T& t) {
        return t;
    }

    template <typename T>
    inline InsertAfterObject<T> insert_after(const T& t) {
        return t;
    }

    template <typename T>
    inline InsertBetweenObject<T> insert_between(const T& t) {
        return t;
    }

    template <typename T>
    inline InsertAroundObject<T> insert_around(const T& t1, const T& t2) {
        return {t1, t2};
    }

    // permutations

    template <typename ComparisonPredicate>
    struct PermutationsObject:
    AlgorithmBase<PermutationsObject<ComparisonPredicate>> {
        ComparisonPredicate comp;
        PermutationsObject() = default;
        PermutationsObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> PermutationsObject<CP2> operator()(CP2 p) const { return p; }
    };

    template <typename Range, typename ComparisonPredicate>
    class PermutationsIterator:
    public TL::InputIterator<PermutationsIterator<Range, ComparisonPredicate>,
        const Detail::SharedRange<TL::RangeValue<Range>>> {
    public:
        using range_iterator = TL::RangeIterator<const Range>;
        PermutationsIterator() = default;
        PermutationsIterator(range_iterator i, range_iterator j, ComparisonPredicate p):
            val_(Detail::make_shared_range<TL::RangeValue<Range>>(i, j)),
            comp_(p), count_(i == j ? npos : 0) {}
        const auto& operator*() const noexcept { return val_; }
        PermutationsIterator& operator++() {
            if (std::next_permutation(val_.begin(), val_.end(), comp_))
                ++count_;
            else
                count_ = npos;
            return *this;
        }
        bool operator==(const PermutationsIterator& rhs) const noexcept { return count_ == rhs.count_; }
    private:
        Detail::SharedRange<TL::RangeValue<Range>> val_;
        ComparisonPredicate comp_;
        size_t count_ = npos;
    };

    template <typename Range, typename ComparisonPredicate>
    TL::Irange<PermutationsIterator<Range, ComparisonPredicate>>
    operator>>(const Range& lhs, PermutationsObject<ComparisonPredicate> rhs) {
        auto r = Detail::as_range(lhs);
        return {{r.begin(), r.end(), rhs.comp}, {}};
    }

    constexpr PermutationsObject<std::less<>> permutations = {};

    // repeat

    struct RepeatObject:
    AlgorithmBase<RepeatObject> {
        ptrdiff_t num = std::numeric_limits<ptrdiff_t>::max();
        RepeatObject operator()(size_t k) const noexcept {
            RepeatObject o;
            if (k < size_t(o.num))
                o.num = k;
            return o;
        }
    };

    template <typename FR>
    class RepeatIterator:
    public Detail::FlexibleIterator<RepeatIterator<FR>, const TL::RangeValue<FR>> {
    public:
        static_assert(Detail::category_is_at_least<FR, std::forward_iterator_tag>);
        using underlying_iterator = TL::RangeIterator<const FR>;
        using iterator_category = Detail::CommonCategory<FR, std::bidirectional_iterator_tag>;
        RepeatIterator() = default;
        RepeatIterator(underlying_iterator b, underlying_iterator e, ptrdiff_t c):
            begin_(b), end_(e), iter_(b), cycle_(c) {}
        const auto& operator*() const noexcept { return *iter_; }
        RepeatIterator& operator++() {
            ++iter_;
            if (iter_ == end_) {
                iter_ = begin_;
                ++cycle_;
            }
            return *this;
        }
        RepeatIterator& operator--() {
            if (iter_ == begin_) {
                iter_ = end_;
                --cycle_;
            }
            --iter_;
            return *this;
        }
        RepeatIterator& operator+=(ptrdiff_t rhs) {
            ptrdiff_t offset = std::distance(begin_, iter_) + rhs;
            ptrdiff_t divisor = std::distance(begin_, end_);
            ptrdiff_t quo = offset / divisor;
            ptrdiff_t rem = offset % divisor;
            if (rem < 0) {
                --quo;
                rem += divisor;
            }
            cycle_ += quo;
            iter_ = begin_;
            std::advance(iter_, rem);
            return *this;
        }
        ptrdiff_t operator-(const RepeatIterator& rhs) const {
            return (cycle_ - rhs.cycle_) * std::distance(begin_, end_) + std::distance(rhs.iter_, iter_);
        }
        bool operator==(const RepeatIterator& rhs) const noexcept {
            return cycle_ == rhs.cycle_ && iter_ == rhs.iter_;
        }
    private:
        underlying_iterator begin_, end_, iter_;
        ptrdiff_t cycle_ = 0;
    };

    template <typename FR>
    TL::Irange<RepeatIterator<FR>> operator>>(const FR& lhs, RepeatObject rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, 0}, {b, e, rhs.num}};
    }

    template <typename Container>
    Container& operator<<(Container& lhs, RepeatObject rhs) {
        if (rhs.num == 0) {
            lhs.clear();
        } else if (rhs.num > 1) {
            Container temp = lhs;
            for (ptrdiff_t i = 1; i < rhs.num; ++i)
                std::copy(lhs.begin(), lhs.end(), append(temp));
            lhs = std::move(temp);
        }
        return lhs;
    }

    constexpr RepeatObject repeat = {};

    // subsets

    struct SubsetsObject:
    AlgorithmBase<SubsetsObject> {
        size_t num;
        SubsetsObject(size_t k): num(k) {}
    };

    template <typename ForwardRange>
    class SubsetsIterator:
    public TL::InputIterator<SubsetsIterator<ForwardRange>,
        const Detail::SharedRange<TL::RangeValue<ForwardRange>>> {
    public:
        using range_iterator = TL::RangeIterator<const ForwardRange>;
        using value_type = TL::RangeValue<ForwardRange>;
        SubsetsIterator() = default;
        SubsetsIterator(range_iterator i, range_iterator j, size_t k):
            its_(Detail::make_shared_range<range_iterator>(std::min(k, size_t(std::distance(i, j))))),
            val_(Detail::make_shared_range<value_type>(std::distance(its_.begin(), its_.end()))),
            src_{i, j},
            count_(0) {
                auto& ivec = *its_.first.share;
                auto& vvec = *val_.first.share;
                for (size_t m = 0, n = ivec.size(); m < n; ++i, ++m) {
                    ivec[m] = i;
                    vvec[m] = *i;
                }
            }
        const auto& operator*() const noexcept { return val_; }
        SubsetsIterator& operator++() {
            if (count_ == npos)
                return *this;
            auto& ivec = *its_.first.share;
            auto& vvec = *val_.first.share;
            size_t i = ivec.size(), j = 0;
            for (; i > 0; --i) {
                auto k = ivec[i - 1];
                for (j = i - 1; j < ivec.size(); ++j) {
                    ivec[j] = ++k;
                    if (k == src_.second)
                        break;
                    vvec[j] = *k;
                }
                if (j == ivec.size())
                    break;
            }
            if (i == 0)
                count_ = npos;
            else
                ++count_;
            return *this;
        }
        bool operator==(const SubsetsIterator& rhs) const noexcept { return count_ == rhs.count_; }
    private:
        Detail::SharedRange<range_iterator> its_;
        Detail::SharedRange<value_type> val_;
        TL::Irange<range_iterator> src_;
        size_t count_ = npos;
    };

    template <typename ForwardRange>
    TL::Irange<SubsetsIterator<ForwardRange>>
    operator>>(const ForwardRange& lhs, SubsetsObject rhs) {
        auto r = Detail::as_range(lhs);
        return {{r.begin(), r.end(), rhs.num}, {}};
    }

    inline SubsetsObject subsets(size_t k) { return {k}; }

}
