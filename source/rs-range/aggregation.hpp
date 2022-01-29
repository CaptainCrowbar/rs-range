#pragma once

#include "rs-range/core.hpp"
#include "rs-tl/iterator.hpp"
#include <algorithm>
#include <functional>
#include <numeric>
#include <type_traits>
#include <utility>

namespace RS::Range {

    // adjacent_difference

    template <typename BinaryFunction>
    struct AdjacentDifferenceObject:
    AlgorithmBase<AdjacentDifferenceObject<BinaryFunction>> {
        BinaryFunction fun;
        AdjacentDifferenceObject() = default;
        AdjacentDifferenceObject(const BinaryFunction& f): fun(f) {}
        template <typename BF2> AdjacentDifferenceObject<BF2> operator()(BF2 f) const { return f; }
    };

    template <typename Range, typename BinaryFunction>
    class AdjacentDifferenceIterator:
    public TL::ForwardIterator<AdjacentDifferenceIterator<Range, BinaryFunction>,
        const std::invoke_result_t<BinaryFunction, TL::RangeValue<Range>, TL::RangeValue<Range>>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = std::invoke_result_t<BinaryFunction, TL::RangeValue<Range>, TL::RangeValue<Range>>;
        using function_type = std::function<value_type(const TL::RangeValue<Range>&, const TL::RangeValue<Range>&)>;
        AdjacentDifferenceIterator() = default;
        AdjacentDifferenceIterator(underlying_iterator b, underlying_iterator e, BinaryFunction f):
            iter1_(b), iter2_(b), fun_(f), value_(), ok_(false) { if (b != e) ++iter2_; }
        const value_type& operator*() const noexcept { if (! ok_) { value_ = fun_(*iter2_, *iter1_); ok_ = true; } return value_; };
        AdjacentDifferenceIterator& operator++() { iter1_ = iter2_; ++iter2_; ok_ = false; return *this; }
        bool operator==(const AdjacentDifferenceIterator& rhs) const noexcept { return iter2_ == rhs.iter2_; }
    private:
        underlying_iterator iter1_, iter2_;
        function_type fun_;
        mutable value_type value_;
        mutable bool ok_;
    };

    template <typename Range, typename BinaryFunction>
    TL::Irange<AdjacentDifferenceIterator<Range, BinaryFunction>>
    operator>>(const Range& lhs, AdjacentDifferenceObject<BinaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.fun}, {e, e, rhs.fun}};
    }

    template <typename Container, typename BinaryFunction>
    Container& operator<<(Container& lhs, AdjacentDifferenceObject<BinaryFunction> rhs) {
        Container temp;
        lhs >> rhs >> append(temp);
        lhs = std::move(temp);
        return lhs;
    }

    constexpr AdjacentDifferenceObject<std::minus<>> adjacent_difference = {};

    // census

    template <typename EqualityPredicate>
    struct CensusObject:
    AlgorithmBase<CensusObject<EqualityPredicate>> {
        EqualityPredicate eq;
        CensusObject() = default;
        CensusObject(const EqualityPredicate& p): eq(p) {}
        template <typename EP2> CensusObject<EP2> operator()(EP2 p) const { return p; }
    };

    template <typename Range, typename EqualityPredicate>
    class CensusIterator:
    public TL::ForwardIterator<CensusIterator<Range, EqualityPredicate>, const std::pair<TL::RangeValue<Range>, size_t>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = std::pair<TL::RangeValue<Range>, size_t>;
        using equality_predicate = std::function<bool(const TL::RangeValue<Range>&, const TL::RangeValue<Range>&)>;
        CensusIterator() = default;
        CensusIterator(underlying_iterator b, underlying_iterator e, EqualityPredicate p):
            iter_(b), end_(e), eq_(p), value_() { next(); }
        const value_type& operator*() const noexcept { return value_; }
        CensusIterator& operator++() { next(); return *this; }
        bool operator==(const CensusIterator& rhs) const noexcept { return iter_ == rhs.iter_ && value_.second == rhs.value_.second; }
    private:
        underlying_iterator iter_, end_;
        equality_predicate eq_;
        value_type value_;
        void next() {
            value_.second = 0;
            if (iter_ == end_)
                return;
            value_.first = *iter_;
            do {
                ++iter_;
                ++value_.second;
            } while (iter_ != end_ && eq_(*iter_, value_.first));
        }
    };

    template <typename Range, typename EqualityPredicate>
    TL::Irange<CensusIterator<Range, EqualityPredicate>> operator>>(const Range& lhs, CensusObject<EqualityPredicate> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.eq}, {e, e, rhs.eq}};
    }

    template <typename Container, typename EqualityPredicate>
    Container& operator<<(Container& lhs, CensusObject<EqualityPredicate> rhs) {
        std::partial_sum(lhs.begin(), lhs.end(), lhs.begin(), rhs.eq);
        return lhs;
    }

    constexpr CensusObject<std::equal_to<>> census = {};

    // collect_groups, collect_groups_by

    template <typename BinaryPredicate, typename BinaryFunction>
    struct CollectGroupsObject:
    AlgorithmBase<CollectGroupsObject<BinaryPredicate, BinaryFunction>> {
        BinaryPredicate pred;
        BinaryFunction func;
        CollectGroupsObject() = default;
        CollectGroupsObject(const BinaryPredicate& bp, const BinaryFunction& bf): pred(bp), func(bf) {}
    };

    template <typename Range, typename BinaryPredicate, typename BinaryFunction>
    class CollectGroupsIterator:
    public TL::ForwardIterator<CollectGroupsIterator<Range, BinaryPredicate, BinaryFunction>, const TL::RangeValue<Range>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::RangeValue<Range>;
        using predicate_type = std::function<bool(const value_type&, const value_type&)>;
        using function_type = std::function<value_type(const value_type&, const value_type&)>;
        CollectGroupsIterator() = default;
        CollectGroupsIterator(underlying_iterator b, underlying_iterator e, BinaryPredicate bp, BinaryFunction bf):
            pred_(bp), func_(bf), value_(), iter_(b), next_(b), end_(e) { advance(); }
        const value_type& operator*() const noexcept { return value_; }
        CollectGroupsIterator& operator++() { advance(); return *this; }
        bool operator==(const CollectGroupsIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        predicate_type pred_;
        function_type func_;
        value_type value_;
        underlying_iterator iter_;
        underlying_iterator next_;
        underlying_iterator end_;
        void advance() {
            iter_ = next_;
            if (iter_ == end_)
                return;
            value_ = *iter_;
            for (;;) {
                ++next_;
                if (next_ == end_ || ! pred_(value_, *next_))
                    break;
                value_ = func_(value_, *next_);
            }
        }
    };

    template <typename Range, typename BinaryPredicate, typename BinaryFunction>
    TL::Irange<CollectGroupsIterator<Range, BinaryPredicate, BinaryFunction>>
    operator>>(const Range& lhs, CollectGroupsObject<BinaryPredicate, BinaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.pred, rhs.func}, {e, e, rhs.pred, rhs.func}};
    }

    template <typename Container, typename BinaryPredicate, typename BinaryFunction>
    Container& operator<<(Container& lhs, CollectGroupsObject<BinaryPredicate, BinaryFunction> rhs) {
        using std::swap;
        Container result;
        lhs >> rhs >> append(result);
        swap(lhs, result);
        return lhs;
    }

    template <typename BinaryPredicate>
    inline CollectGroupsObject<BinaryPredicate, std::plus<>> collect_groups(BinaryPredicate bp) {
        return {bp, {}};
    }

    template <typename BinaryPredicate, typename BinaryFunction>
    inline CollectGroupsObject<BinaryPredicate, BinaryFunction> collect_groups(BinaryPredicate bp, BinaryFunction bf) {
        return {bp, bf};
    }

    template <typename UnaryFunction>
    inline auto collect_groups_by(UnaryFunction uf) {
        auto bp = [uf] (auto& a, auto& b) { return uf(a) == uf(b); };
        return collect_groups(bp);
    }

    template <typename UnaryFunction, typename BinaryFunction>
    inline auto collect_groups_by(UnaryFunction uf, BinaryFunction bf) {
        auto bp = [uf] (auto& a, auto& b) { return uf(a) == uf(b); };
        return collect_groups(bp, bf);
    }

    // group

    template <typename EqualityPredicate>
    struct GroupObject:
    AlgorithmBase<GroupObject<EqualityPredicate>> {
        EqualityPredicate pred;
        GroupObject() = default;
        GroupObject(const EqualityPredicate& p): pred(p) {}
        template <typename EP2> GroupObject<EP2> operator()(EP2 p) const { return p; }
    };

    template <typename Range, typename EqualityPredicate>
    class GroupIterator:
    public TL::ForwardIterator<GroupIterator<Range, EqualityPredicate>, const TL::Irange<TL::RangeIterator<const Range>>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::Irange<underlying_iterator>;
        using predicate_type = std::function<bool(const TL::RangeValue<Range>&, const TL::RangeValue<Range>&)>;
        GroupIterator() = default;
        GroupIterator(underlying_iterator b, underlying_iterator e, EqualityPredicate p):
            value_{b, b}, end_(e), pred_(p) { next(); }
        const value_type& operator*() const noexcept { return value_; }
        GroupIterator& operator++() { next(); return *this; }
        bool operator==(const GroupIterator& rhs) const noexcept { return value_.first == rhs.value_.first; }
    private:
        value_type value_;
        underlying_iterator end_;
        predicate_type pred_;
        void next() {
            value_.first = value_.second;
            if (value_.first == end_)
                return;
            do ++value_.second;
                while (value_.second != end_ && pred_(*value_.first, *value_.second));
        }
    };

    template <typename Range, typename EqualityPredicate>
    TL::Irange<GroupIterator<Range, EqualityPredicate>> operator>>(const Range& lhs, GroupObject<EqualityPredicate> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.pred}, {e, e, rhs.pred}};
    }

    constexpr GroupObject<std::equal_to<>> group = {};

    // group_by

    template <typename UnaryFunction>
    struct GroupByObject:
    AlgorithmBase<GroupByObject<UnaryFunction>> {
        UnaryFunction fun;
        GroupByObject(const UnaryFunction& f): fun(f) {}
    };

    template <typename Range, typename UnaryFunction>
    class GroupByIterator:
    public TL::ForwardIterator<GroupByIterator<Range, UnaryFunction>, const TL::Irange<TL::RangeIterator<const Range>>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::Irange<underlying_iterator>;
        using function_result = std::invoke_result_t<UnaryFunction, TL::RangeValue<Range>>;
        using function_type = std::function<function_result(const TL::RangeValue<Range>&)>;
        GroupByIterator() = default;
        GroupByIterator(underlying_iterator b, underlying_iterator e, UnaryFunction f):
            value_{b, b}, end_(e), fun_(f), current_() {
                if (b != e)
                    current_ = f(*b);
                next();
            }
        const value_type& operator*() const noexcept { return value_; }
        GroupByIterator& operator++() { next(); return *this; }
        bool operator==(const GroupByIterator& rhs) const noexcept { return value_.first == rhs.value_.first; }
    private:
        value_type value_;
        underlying_iterator end_;
        function_type fun_;
        function_result current_;
        void next() {
            value_.first = value_.second;
            if (value_.first == end_)
                return;
            auto prev = current_;
            do {
                ++value_.second;
                if (value_.second == end_)
                    break;
                current_ = fun_(*value_.second);
            } while (current_ == prev);
        }
    };

    template <typename Range, typename UnaryFunction>
    TL::Irange<GroupByIterator<Range, UnaryFunction>> operator>>(const Range& lhs, GroupByObject<UnaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.fun}, {e, e, rhs.fun}};
    }

    template <typename UnaryFunction>
    inline GroupByObject<UnaryFunction> group_by(UnaryFunction f) {
        return f;
    }

    // group_k

    struct GroupKObject:
    AlgorithmBase<GroupKObject> {
        size_t num;
        GroupKObject(size_t k): num(k) {}
    };

    template <typename Range>
    class GroupNIterator:
    public TL::ForwardIterator<GroupNIterator<Range>, const TL::Irange<TL::RangeIterator<const Range>>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::Irange<underlying_iterator>;
        GroupNIterator() = default;
        GroupNIterator(underlying_iterator b, underlying_iterator e, size_t k):
            value_{b, b}, end_(e), num_(k) { next(); }
        const value_type& operator*() const noexcept { return value_; }
        GroupNIterator& operator++() { next(); return *this; }
        bool operator==(const GroupNIterator& rhs) const noexcept { return value_.first == rhs.value_.first; }
    private:
        value_type value_;
        underlying_iterator end_;
        size_t num_;
        void next() {
            value_.first = value_.second;
            if (value_.first != end_)
                for (size_t i = 0; i < num_ && value_.second != end_; ++i, ++value_.second) {}
        }
    };

    template <typename Range>
    TL::Irange<GroupNIterator<Range>> operator>>(const Range& lhs, GroupKObject rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.num}, {e, e, rhs.num}};
    }

    inline GroupKObject group_k(size_t k) {
        return {k == 0 ? 1 : k};
    }

    // partial_sum

    template <typename BinaryFunction>
    struct PartialSumObject:
    AlgorithmBase<PartialSumObject<BinaryFunction>> {
        BinaryFunction fun;
        PartialSumObject() = default;
        PartialSumObject(const BinaryFunction& f): fun(f) {}
        template <typename BF2> PartialSumObject<BF2> operator()(BF2 f) const { return f; }
    };

    template <typename Range, typename BinaryFunction>
    class PartialSumIterator:
    public TL::ForwardIterator<PartialSumIterator<Range, BinaryFunction>,
        const std::invoke_result_t<BinaryFunction, TL::RangeValue<Range>, TL::RangeValue<Range>>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = std::invoke_result_t<BinaryFunction, TL::RangeValue<Range>, TL::RangeValue<Range>>;
        using function_type = std::function<value_type(const TL::RangeValue<Range>&, const TL::RangeValue<Range>&)>;
        PartialSumIterator() = default;
        PartialSumIterator(underlying_iterator b, underlying_iterator e, BinaryFunction f):
            iter_(b), fun_(f), value_(), ok_(true) { if (b != e) value_ = *b; }
        const value_type& operator*() const noexcept { if (! ok_) { value_ = fun_(value_, *iter_); ok_ = true; } return value_; };
        PartialSumIterator& operator++() { ++iter_; ok_ = false; return *this; }
        bool operator==(const PartialSumIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        function_type fun_;
        mutable value_type value_;
        mutable bool ok_;
    };

    template <typename Range, typename BinaryFunction>
    TL::Irange<PartialSumIterator<Range, BinaryFunction>> operator>>(const Range& lhs, PartialSumObject<BinaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.fun}, {e, e, rhs.fun}};
    }

    template <typename Container, typename BinaryFunction>
    Container& operator<<(Container& lhs, PartialSumObject<BinaryFunction> rhs) {
        std::partial_sum(lhs.begin(), lhs.end(), lhs.begin(), rhs.fun);
        return lhs;
    }

    const PartialSumObject<std::plus<>> partial_sum = {};

}
