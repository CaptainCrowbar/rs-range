#pragma once

#include "rs-range/core.hpp"
#include "rs-tl/iterator.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace RS::Range {

    // compare, compare_3way, is_equal, is_equivalent

    template <typename Range2, typename ComparisonPredicate>
    struct CompareObject:
    AlgorithmBase<CompareObject<Range2, ComparisonPredicate>> {
        Detail::AsRange<const Range2> range;
        ComparisonPredicate comp;
        CompareObject(const Range2& r2, const ComparisonPredicate& cp): range(r2), comp(cp) {}
        template <typename CP2> CompareObject<Range2, CP2> operator()(const CP2& cp) const { return {range, cp}; }
    };

    template <typename Range2, typename ComparisonPredicate>
    struct Compare3WayObject:
    AlgorithmBase<Compare3WayObject<Range2, ComparisonPredicate>> {
        Detail::AsRange<const Range2> range;
        ComparisonPredicate comp;
        Compare3WayObject(const Range2& r2, const ComparisonPredicate& cp): range(r2), comp(cp) {}
        template <typename CP2> Compare3WayObject<Range2, CP2> operator()(const CP2& cp) const { return {range, cp}; }
    };

    template <typename Range2, typename EqualityPredicate>
    struct IsEqualObject:
    AlgorithmBase<IsEqualObject<Range2, EqualityPredicate>> {
        Detail::AsRange<const Range2> range;
        EqualityPredicate comp;
        IsEqualObject(const Range2& r2, const EqualityPredicate& cp): range(r2), comp(cp) {}
        template <typename CP2> IsEqualObject<Range2, CP2> operator()(const CP2& cp) const { return {range, cp}; }
    };

    template <typename Range2, typename ComparisonPredicate>
    struct IsEquivalentObject:
    AlgorithmBase<IsEquivalentObject<Range2, ComparisonPredicate>> {
        Detail::AsRange<const Range2> range;
        ComparisonPredicate comp;
        IsEquivalentObject(const Range2& r2, const ComparisonPredicate& cp): range(r2), comp(cp) {}
        template <typename CP2> IsEquivalentObject<Range2, CP2> operator()(const CP2& cp) const { return {range, cp}; }
    };

    template <typename Range1, typename Range2, typename ComparisonPredicate>
    bool operator>>(const Range1& lhs, const CompareObject<Range2, ComparisonPredicate>& rhs) {
        auto r = Detail::as_range(lhs);
        auto i = r.begin(), ie = r.end();
        auto j = rhs.range.begin(), je = rhs.range.end();
        for (; i != ie && j != je; ++i, ++j) {
            if (rhs.comp(*i, *j))
                return true;
            else if (rhs.comp(*j, *i))
                return false;
        }
        return j != je;
    }

    template <typename Range1, typename Range2, typename ComparisonPredicate>
    int operator>>(const Range1& lhs, const Compare3WayObject<Range2, ComparisonPredicate>& rhs) {
        auto r = Detail::as_range(lhs);
        auto i = r.begin(), ie = r.end();
        auto j = rhs.range.begin(), je = rhs.range.end();
        for (; i != ie && j != je; ++i, ++j) {
            if (rhs.comp(*i, *j))
                return -1;
            else if (rhs.comp(*j, *i))
                return 1;
        }
        if (i != ie)
            return 1;
        else if (j != je)
            return -1;
        else
            return 0;
    }

    template <typename Range1, typename Range2, typename EqualityPredicate>
    bool operator>>(const Range1& lhs, const IsEqualObject<Range2, EqualityPredicate>& rhs) {
        auto r = Detail::as_range(lhs);
        static constexpr bool ra1 = Detail::category_equals<Range1, std::random_access_iterator_tag>;
        static constexpr bool ra2 = Detail::category_equals<Range2, std::random_access_iterator_tag>;
        auto i = r.begin(), ie = r.end();
        auto j = rhs.range.begin(), je = rhs.range.end();
        if (ra1 && ra2 && std::distance(i, ie) != std::distance(j, je))
            return false;
        for (; i != ie && j != je; ++i, ++j)
            if (! rhs.comp(*i, *j))
                return false;
        return i == ie && j == je;
    }

    template <typename Range1, typename Range2, typename ComparisonPredicate>
    bool operator>>(const Range1& lhs, const IsEquivalentObject<Range2, ComparisonPredicate>& rhs) {
        auto r = Detail::as_range(lhs);
        static constexpr bool ra1 = Detail::category_equals<Range1, std::random_access_iterator_tag>;
        static constexpr bool ra2 = Detail::category_equals<Range2, std::random_access_iterator_tag>;
        auto i = r.begin(), ie = r.end();
        auto j = rhs.range.begin(), je = rhs.range.end();
        if (ra1 && ra2 && std::distance(i, ie) != std::distance(j, je))
            return false;
        for (; i != ie && j != je; ++i, ++j)
            if (rhs.comp(*i, *j) || rhs.comp(*j, *i))
                return false;
        return i == ie && j == je;
    }

    template <typename Range2>
    inline CompareObject<Range2, std::less<>> compare(Range2& r) {
        return {r, {}};
    }

    template <typename Range2>
    inline Compare3WayObject<Range2, std::less<>> compare_3way(Range2& r) {
        return {r, {}};
    }

    template <typename Range2>
    inline IsEqualObject<Range2, std::equal_to<>> is_equal(Range2& r) {
        return {r, {}};
    }

    template <typename Range2>
    inline IsEquivalentObject<Range2, std::less<>> is_equivalent(Range2& r) {
        return {r, {}};
    }

    // concat, prefix

    template <typename Range2>
    struct ConcatObject:
    AlgorithmBase<ConcatObject<Range2>> {
        Detail::AsRange<const Range2> range;
        ConcatObject(const Range2& r): range(r) {}
    };

    template <typename Range2>
    struct PrefixObject:
    AlgorithmBase<PrefixObject<Range2>> {
        Detail::AsRange<const Range2> range;
        PrefixObject(const Range2& r): range(r) {}
    };

    template <typename Range1, typename Range2>
    class ConcatIterator:
    public TL::ForwardIterator<ConcatIterator<Range1, Range2>, const TL::RangeValue<Range1>> {
    public:
        static_assert(std::is_same<TL::RangeValue<Range1>, TL::RangeValue<Range2>>::value);
        using left_iterator = TL::RangeIterator<const Range1>;
        using right_iterator = TL::RangeIterator<const Range2>;
        using iterator_category = Detail::CommonCategory<Range1, Range2, std::forward_iterator_tag>;
        ConcatIterator() = default;
        ConcatIterator(left_iterator b1, left_iterator e1, right_iterator b2): left_(b1), left_end_(e1), right_(b2) {}
        const auto& operator*() const noexcept { return on_left() ? *left_ : *right_; }
        ConcatIterator& operator++() { if (on_left()) ++left_; else ++right_; return *this; }
        bool operator==(const ConcatIterator& rhs) const noexcept { return left_ == rhs.left_ && right_ == rhs.right_; }
    private:
        left_iterator left_, left_end_;
        right_iterator right_;
        bool on_left() const noexcept { return left_ != left_end_; }
    };

    template <typename Range1, typename Range2>
    TL::Irange<ConcatIterator<Range1, Range2>> operator>>(const Range1& lhs, ConcatObject<Range2> rhs) {
        auto r = Detail::as_range(lhs);
        auto b1 = r.begin(), e1 = r.end();
        auto b2 = rhs.range.begin(), e2 = rhs.range.end();
        return {{b1, e1, b2}, {e1, e1, e2}};
    }

    template <typename Range1, typename Range2>
    TL::Irange<ConcatIterator<Range2, Range1>> operator>>(const Range1& lhs, PrefixObject<Range2> rhs) {
        auto r = Detail::as_range(lhs);
        auto b1 = r.begin(), e1 = r.end();
        auto b2 = rhs.range.begin(), e2 = rhs.range.end();
        return {{b2, e2, b1}, {e2, e2, e1}};
    }

    template <typename Container, typename Range2>
    Container& operator<<(Container& lhs, ConcatObject<Range2> rhs) {
        lhs.insert(lhs.end(), rhs.range.begin(), rhs.range.end());
        return lhs;
    }

    template <typename Container, typename Range2>
    Container& operator<<(Container& lhs, PrefixObject<Range2> rhs) {
        lhs.insert(lhs.begin(), rhs.range.begin(), rhs.range.end());
        return lhs;
    }

    template <typename Range2>
    inline ConcatObject<Range2> concat(Range2& r) {
        return {r};
    }

    template <typename Range2>
    inline PrefixObject<Range2> prefix(Range2& r) {
        return {r};
    }

    // inner_product

    template <typename Range, typename T, typename BinaryFunction1, typename BinaryFunction2>
    struct InnerProductObject:
    AlgorithmBase<InnerProductObject<Range, T, BinaryFunction1, BinaryFunction2>> {
        Detail::AsRange<const Range> range;
        T init;
        BinaryFunction1 mul;
        BinaryFunction2 add;
        InnerProductObject(const Range& r, const T& t): range(r), init(t), mul(), add() {}
        InnerProductObject(const Range& r, const T& t, const BinaryFunction1& m, const BinaryFunction2& a):
            range(r), init(t), mul(m), add(a) {}
    };

    template <typename Range1, typename Range2, typename T, typename BinaryFunction1, typename BinaryFunction2>
    auto operator>>(const Range1& lhs, const InnerProductObject<Range2, T, BinaryFunction1, BinaryFunction2>& rhs) {
        auto r = Detail::as_range(lhs);
        auto i = r.begin(), e1 = r.end();
        auto j = rhs.range.begin(), e2 = rhs.range.end();
        T result = rhs.init;
        for (; i != e1 && j != e2; ++i, ++j)
            result = rhs.add(result, rhs.mul(*i, *j));
        return result;
    }

    template <typename Range>
    inline InnerProductObject<Range, TL::RangeValue<Range>, std::multiplies<>, std::plus<>> inner_product(const Range& r) {
        return {r, TL::RangeValue<Range>()};
    }

    template <typename Range, typename T, typename BinaryFunction1, typename BinaryFunction2>
    inline InnerProductObject<Range, T, BinaryFunction1, BinaryFunction2>
    inner_product(const Range& r, const T& t, BinaryFunction1 f1, BinaryFunction2 f2) {
        return {r, t, f1, f2};
    }

    // interleave

    template <typename Range2>
    struct InterleaveObject:
    AlgorithmBase<InterleaveObject<Range2>> {
        Detail::AsRange<const Range2> range;
        InterleaveObject(const Range2& r): range(r) {}
    };

    template <typename Range1, typename Range2>
    class InterleaveIterator:
    public TL::ForwardIterator<InterleaveIterator<Range1, Range2>, const TL::RangeValue<Range1>> {
    public:
        static_assert(std::is_same<TL::RangeValue<Range1>, TL::RangeValue<Range2>>::value);
        using left_iterator = TL::RangeIterator<const Range1>;
        using right_iterator = TL::RangeIterator<const Range2>;
        using iterator_category = Detail::CommonCategory<Range1, Range2, std::forward_iterator_tag>;
        InterleaveIterator() = default;
        InterleaveIterator(left_iterator b1, left_iterator e1, right_iterator b2, right_iterator e2):
            left_(b1), left_end_(e1), right_(b2), right_end_(e2), on_left_(b1 != e1 || b2 == e2) {}
        const auto& operator*() const noexcept { return on_left_ ? *left_ : *right_; }
        InterleaveIterator& operator++() {
            if (on_left_) {
                if (left_ != left_end_)
                    ++left_;
                on_left_ = right_ == right_end_;
            } else {
                if (right_ != right_end_)
                    ++right_;
                on_left_ = left_ != left_end_ || right_ == right_end_;
            }
            return *this;
        }
        bool operator==(const InterleaveIterator& rhs) const noexcept
            { return left_ == rhs.left_ && right_ == rhs.right_ && on_left_ == rhs.on_left_; }
    private:
        left_iterator left_, left_end_;
        right_iterator right_, right_end_;
        bool on_left_;
    };

    template <typename Range1, typename Range2>
    TL::Irange<InterleaveIterator<Range1, Range2>> operator>>(const Range1& lhs, InterleaveObject<Range2> rhs) {
        auto r = Detail::as_range(lhs);
        auto b1 = r.begin(), e1 = r.end();
        auto b2 = rhs.range.begin(), e2 = rhs.range.end();
        return {{b1, e1, b2, e2}, {e1, e1, e2, e2}};
    }

    template <typename Container, typename Range2>
    Container& operator<<(Container& lhs, InterleaveObject<Range2> rhs) {
        Container temp;
        lhs >> rhs >> append(temp);
        lhs = std::move(temp);
        return lhs;
    }

    template <typename Range2>
    inline InterleaveObject<Range2> interleave(Range2& r) {
        return {r};
    }

    // merge

    template <typename SortedRange2, typename ComparisonPredicate>
    struct MergeObject:
    AlgorithmBase<MergeObject<SortedRange2, ComparisonPredicate>> {
        Detail::AsRange<const SortedRange2> range;
        ComparisonPredicate comp;
        MergeObject(const SortedRange2& r, const ComparisonPredicate& c): range(r), comp(c) {}
    };

    template <typename SortedRange1, typename SortedRange2, typename ComparisonPredicate>
    class MergeIterator:
    public TL::ForwardIterator<MergeIterator<SortedRange1, SortedRange2, ComparisonPredicate>, const TL::RangeValue<SortedRange1>> {
    public:
        static_assert(std::is_same<TL::RangeValue<SortedRange1>, TL::RangeValue<SortedRange2>>::value);
        using left_iterator = TL::RangeIterator<const SortedRange1>;
        using right_iterator = TL::RangeIterator<const SortedRange2>;
        using iterator_category = Detail::CommonCategory<SortedRange1, SortedRange2, std::forward_iterator_tag>;
        using left_value = TL::RangeValue<SortedRange1>;
        using right_value = TL::RangeValue<SortedRange2>;
        using predicate_type = std::function<bool(const left_value&, const right_value&)>;
        MergeIterator() = default;
        MergeIterator(left_iterator b1, left_iterator e1, right_iterator b2, right_iterator e2, ComparisonPredicate p):
            it1_(b1), end1_(e1), it2_(b2), end2_(e2), comp_(p), left_() { update(); }
        const auto& operator*() const noexcept { return left_ ? *it1_ : *it2_; }
        MergeIterator& operator++() {
            if (it2_ == end2_)
                ++it1_;
            else if (it1_ == end1_)
                ++it2_;
            else if (left_)
                ++it1_;
            else
                ++it2_;
            update();
            return *this;
        }
        bool operator==(const MergeIterator& rhs) const noexcept { return it1_ == rhs.it1_ && it2_ == rhs.it2_; }
    private:
        left_iterator it1_, end1_;
        right_iterator it2_, end2_;
        predicate_type comp_;
        bool left_;
        void update() { left_ = it1_ != end1_ && (it2_ == end2_ || comp_(*it1_, *it2_)); }
    };

    template <typename SortedRange1, typename SortedRange2, typename ComparisonPredicate>
    TL::Irange<MergeIterator<SortedRange1, SortedRange2, ComparisonPredicate>>
    operator>>(const SortedRange1& lhs, MergeObject<SortedRange2, ComparisonPredicate> rhs) {
        auto r = Detail::as_range(lhs);
        auto b1 = r.begin(), e1 = r.end();
        auto b2 = rhs.range.begin(), e2 = rhs.range.end();
        return {{b1, e1, b2, e2, rhs.comp}, {e1, e1, e2, e2, rhs.comp}};
    }

    template <typename Container, typename SortedRange2, typename ComparisonPredicate>
    Container& operator<<(Container& lhs, MergeObject<SortedRange2, ComparisonPredicate> rhs) {
        Container temp;
        std::merge(lhs.begin(), lhs.end(), rhs.range.begin(), rhs.range.end(), append(temp), rhs.comp);
        lhs = std::move(temp);
        return lhs;
    }

    template <typename SortedRange2>
    inline MergeObject<SortedRange2, std::less<>> merge(SortedRange2& r) {
        return {r, {}};
    }

    template <typename SortedRange2, typename ComparisonPredicate>
    inline MergeObject<SortedRange2, ComparisonPredicate> merge(SortedRange2& r, ComparisonPredicate p) {
        return {r, p};
    }

    // outer_product, self_cross

    template <typename ForwardRange, typename BinaryFunction>
    struct OuterProductObject:
    AlgorithmBase<OuterProductObject<ForwardRange, BinaryFunction>> {
        Detail::AsRange<const ForwardRange> range;
        BinaryFunction fun;
        OuterProductObject(const ForwardRange& r, const BinaryFunction& f): range(r), fun(f) {}
        BinaryFunction get_fun() const { return fun; }
    };

    template <typename ForwardRange>
    struct OuterProductObject<ForwardRange, void>:
    AlgorithmBase<OuterProductObject<ForwardRange, void>> {
        Detail::AsRange<const ForwardRange> range;
        OuterProductObject(const ForwardRange& r): range(r) {}
        auto get_fun() const { return [] (const auto& a, const auto& b) { return std::make_pair(a, b); }; }
    };

    template <typename BinaryFunction>
    struct OuterProductObject<void, BinaryFunction>:
    AlgorithmBase<OuterProductObject<void, BinaryFunction>> {
        BinaryFunction fun;
        OuterProductObject(const BinaryFunction& f): fun(f) {}
        BinaryFunction get_fun() const { return fun; }
    };

    template <>
    struct OuterProductObject<void, void>:
    AlgorithmBase<OuterProductObject<void, void>> {
        template <typename BinaryFunction> OuterProductObject<void, BinaryFunction>
            operator()(const BinaryFunction& f) const { return {f}; }
        auto get_fun() const { return [] (const auto& a, const auto& b) { return std::make_pair(a, b); }; }
    };

    template <typename ForwardRange1, typename ForwardRange2, typename BinaryFunction>
    class OuterProductIterator:
    public TL::ForwardIterator<OuterProductIterator<ForwardRange1, ForwardRange2, BinaryFunction>,
        const Detail::PairResult<ForwardRange1, ForwardRange2, BinaryFunction>> {
    public:
        static_assert(Detail::category_is_at_least<ForwardRange1, std::forward_iterator_tag>);
        static_assert(Detail::category_is_at_least<ForwardRange2, std::forward_iterator_tag>);
        using value_type = Detail::PairResult<ForwardRange1, ForwardRange2, BinaryFunction>;
        using left_iterator = TL::RangeIterator<const ForwardRange1>;
        using right_iterator = TL::RangeIterator<const ForwardRange2>;
        using function_type = std::function<value_type(const TL::RangeValue<ForwardRange1>&, const TL::RangeValue<ForwardRange2>&)>;
        using product_object = OuterProductObject<ForwardRange2, BinaryFunction>;
        OuterProductIterator() = default;
        template <typename BinaryFunction2>
            OuterProductIterator(left_iterator b1, left_iterator e1, right_iterator b2, right_iterator e2, const BinaryFunction2& f):
            left_(b1), left_end_(e1), right_(b2), right_begin_(b2), right_end_(e2), fun_(f), value_() { update(); }
        const value_type& operator*() const noexcept { return value_; }
        OuterProductIterator& operator++() {
            if (++right_ == right_end_) {
                right_ = right_begin_;
                ++left_;
            }
            update();
            return *this;
        }
        bool operator==(const OuterProductIterator& rhs) const noexcept { return left_ == rhs.left_ || right_ == rhs.right_; }
    private:
        left_iterator left_, left_end_;
        right_iterator right_, right_begin_, right_end_;
        function_type fun_;
        value_type value_;
        void update() { if (left_ != left_end_ && right_ != right_end_) value_ = fun_(*left_, *right_); }
    };

    template <typename ForwardRange1, typename ForwardRange2, typename BinaryFunction>
    TL::Irange<OuterProductIterator<ForwardRange1, ForwardRange2, BinaryFunction>>
    operator>>(const ForwardRange1& lhs, OuterProductObject<ForwardRange2, BinaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        auto b1 = r.begin(), e1 = r.end();
        auto b2 = rhs.range.begin(), e2 = rhs.range.end();
        return {{b1, e1, b2, e2, rhs.get_fun()}, {e1, e1, e2, e2, rhs.get_fun()}};
    }

    template <typename ForwardRange, typename BinaryFunction>
    TL::Irange<OuterProductIterator<ForwardRange, ForwardRange, BinaryFunction>>
    operator>>(const ForwardRange& lhs, OuterProductObject<void, BinaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, b, e, rhs.get_fun()}, {e, e, e, e, rhs.get_fun()}};
    }

    template <typename ForwardRange2>
    inline OuterProductObject<ForwardRange2, void> outer_product(const ForwardRange2& r) {
        return {r};
    }

    template <typename ForwardRange2, typename BinaryFunction>
    inline OuterProductObject<ForwardRange2, BinaryFunction> outer_product(const ForwardRange2& r, BinaryFunction f) {
        return {r, f};
    }

    constexpr OuterProductObject<void, void> self_cross = {};

    // set algorithms

    struct SetDifferenceTraits {
        static constexpr bool left = true;
        static constexpr bool right = false;
        static constexpr bool both = false;
    };

    struct SetDifferenceFromTraits {
        static constexpr bool left = false;
        static constexpr bool right = true;
        static constexpr bool both = false;
    };

    struct SetIntersectionTraits {
        static constexpr bool left = false;
        static constexpr bool right = false;
        static constexpr bool both = true;
    };

    struct SetSymmetricDifferenceTraits {
        static constexpr bool left = true;
        static constexpr bool right = true;
        static constexpr bool both = false;
    };

    struct SetUnionTraits {
        static constexpr bool left = true;
        static constexpr bool right = true;
        static constexpr bool both = true;
    };

    template <typename SortedRange2, typename ComparisonPredicate, typename SetTraits>
    struct SetOperationObject:
    AlgorithmBase<SetOperationObject<SortedRange2, ComparisonPredicate, SetTraits>> {
        Detail::AsRange<const SortedRange2> range;
        ComparisonPredicate comp;
        SetOperationObject(const SortedRange2& r): range(r), comp() {}
        SetOperationObject(const SortedRange2& r, const ComparisonPredicate& c): range(r), comp(c) {}
    };

    template <typename SortedRange1, typename SortedRange2, typename ComparisonPredicate, typename SetTraits>
    class SetOperationIterator:
    public TL::ForwardIterator<SetOperationIterator<SortedRange1, SortedRange2, ComparisonPredicate, SetTraits>,
        const TL::RangeValue<SortedRange1>> {
    public:
        static_assert(std::is_same<TL::RangeValue<SortedRange1>, TL::RangeValue<SortedRange2>>::value);
        using left_iterator = TL::RangeIterator<const SortedRange1>;
        using right_iterator = TL::RangeIterator<const SortedRange2>;
        using iterator_category = Detail::CommonCategory<SortedRange1, SortedRange2, std::forward_iterator_tag>;
        using left_value = TL::RangeValue<SortedRange1>;
        using right_value = TL::RangeValue<SortedRange2>;
        using predicate_type = std::function<bool(const left_value&, const right_value&)>;
        SetOperationIterator() = default;
        SetOperationIterator(left_iterator b1, left_iterator e1, right_iterator b2, right_iterator e2, ComparisonPredicate p):
            it1_(b1), end1_(e1), it2_(b2), end2_(e2), comp_(p) { if (! accept()) ++*this; }
        const auto& operator*() const noexcept {
            if (it1_ == end1_)             return *it2_;
            else if (it2_ == end2_)        return *it1_;
            else if (comp_(*it2_, *it1_))  return *it2_;
            else                           return *it1_;
        }
        SetOperationIterator& operator++() {
            do {
                if (it1_ == end1_)             ++it2_;
                else if (it2_ == end2_)        ++it1_;
                else if (comp_(*it1_, *it2_))  ++it1_;
                else if (comp_(*it2_, *it1_))  ++it2_;
                else                           { ++it1_; ++it2_; }
            } while (! accept());
            return *this;
        }
        bool operator==(const SetOperationIterator& rhs) const noexcept { return it1_ == rhs.it1_ && it2_ == rhs.it2_; }
    private:
        left_iterator it1_, end1_;
        right_iterator it2_, end2_;
        predicate_type comp_;
        bool accept() const {
            if (it1_ == end1_ && it2_ == end2_)  return true;
            else if (it1_ == end1_)              return SetTraits::right;
            else if (it2_ == end2_)              return SetTraits::left;
            else if (comp_(*it1_, *it2_))        return SetTraits::left;
            else if (comp_(*it2_, *it1_))        return SetTraits::right;
            else                                 return SetTraits::both;
        }
    };

    template <typename SortedRange1, typename SortedRange2, typename ComparisonPredicate, typename SetTraits>
    TL::Irange<SetOperationIterator<SortedRange1, SortedRange2, ComparisonPredicate, SetTraits>>
    operator>>(const SortedRange1& lhs, SetOperationObject<SortedRange2, ComparisonPredicate, SetTraits> rhs) {
        auto r = Detail::as_range(lhs);
        auto b1 = r.begin(), e1 = r.end();
        auto b2 = rhs.range.begin(), e2 = rhs.range.end();
        return {{b1, e1, b2, e2, rhs.comp}, {e1, e1, e2, e2, rhs.comp}};
    }

    template <typename Container, typename SortedRange2, typename ComparisonPredicate, typename SetTraits>
    Container& operator<<(Container& lhs, SetOperationObject<SortedRange2, ComparisonPredicate, SetTraits> rhs) {
        Container temp;
        lhs >> rhs >> append(temp);
        lhs = std::move(temp);
        return lhs;
    }

    template <typename SortedRange2>
    inline SetOperationObject<SortedRange2, std::less<>, SetDifferenceTraits> set_difference(SortedRange2& r) {
        return {r};
    }

    template <typename SortedRange2, typename ComparisonPredicate>
    inline SetOperationObject<SortedRange2, ComparisonPredicate, SetDifferenceTraits> set_difference(SortedRange2& r,
            ComparisonPredicate p) {
        return {r, p};
    }

    template <typename SortedRange2>
    inline SetOperationObject<SortedRange2, std::less<>, SetDifferenceFromTraits> set_difference_from(SortedRange2& r) {
        return {r};
    }

    template <typename SortedRange2, typename ComparisonPredicate>
    inline SetOperationObject<SortedRange2, ComparisonPredicate, SetDifferenceFromTraits> set_difference_from(SortedRange2& r, ComparisonPredicate p) {
        return {r, p};
    }

    template <typename SortedRange2>
    inline SetOperationObject<SortedRange2, std::less<>, SetIntersectionTraits> set_intersection(SortedRange2& r) {
        return {r};
    }

    template <typename SortedRange2, typename ComparisonPredicate>
    inline SetOperationObject<SortedRange2, ComparisonPredicate, SetIntersectionTraits> set_intersection(SortedRange2& r,
            ComparisonPredicate p) {
        return {r, p};
    }

    template <typename SortedRange2>
    inline SetOperationObject<SortedRange2, std::less<>, SetSymmetricDifferenceTraits> set_symmetric_difference(SortedRange2& r) {
        return {r};
    }

    template <typename SortedRange2, typename ComparisonPredicate>
    inline SetOperationObject<SortedRange2, ComparisonPredicate, SetSymmetricDifferenceTraits> set_symmetric_difference(SortedRange2& r, ComparisonPredicate p) {
        return {r, p};
    }

    template <typename SortedRange2>
    inline SetOperationObject<SortedRange2, std::less<>, SetUnionTraits> set_union(SortedRange2& r) {
        return {r};
    }

    template <typename SortedRange2, typename ComparisonPredicate>
    inline SetOperationObject<SortedRange2, ComparisonPredicate, SetUnionTraits> set_union(SortedRange2& r, ComparisonPredicate p) {
        return {r, p};
    }

    // zip

    template <typename Range2, typename BinaryFunction>
    struct ZipObject:
    AlgorithmBase<ZipObject<Range2, BinaryFunction>> {
        Detail::AsRange<const Range2> range;
        BinaryFunction fun;
        ZipObject(const Range2& r, const BinaryFunction& f): range(r), fun(f) {}
        BinaryFunction get_fun() const { return fun; }
    };

    template <typename Range2>
    struct ZipObject<Range2, void>:
    AlgorithmBase<ZipObject<Range2, void>> {
        Detail::AsRange<const Range2> range;
        ZipObject(const Range2& r): range(r) {}
        auto get_fun() const { return [] (const auto& a, const auto& b) { return std::make_pair(a, b); }; }
    };

    template <typename Range1, typename Range2, typename BinaryFunction>
    class ZipIterator:
    public TL::ForwardIterator<ZipIterator<Range1, Range2, BinaryFunction>, const Detail::PairResult<Range1, Range2, BinaryFunction>> {
    public:
        using left_iterator = TL::RangeIterator<const Range1>;
        using right_iterator = TL::RangeIterator<const Range2>;
        using iterator_category = Detail::CommonCategory<Range1, Range2, std::forward_iterator_tag>;
        using value_type = Detail::PairResult<Range1, Range2, BinaryFunction>;
        using function_type = std::function<value_type(const TL::RangeValue<Range1>&, const TL::RangeValue<Range2>&)>;
        using zip_object = ZipObject<Range2, BinaryFunction>;
        ZipIterator() = default;
        ZipIterator(left_iterator b1, left_iterator e1, right_iterator b2, right_iterator e2, zip_object z):
            left_(b1), left_end_(e1), right_(b2), right_end_(e2), fun_(z.get_fun()), value_() { update(); }
        const value_type& operator*() const noexcept { return value_; }
        ZipIterator& operator++() { ++left_; ++right_; update(); return *this; }
        bool operator==(const ZipIterator& rhs) const noexcept { return left_ == rhs.left_ || right_ == rhs.right_; }
    private:
        left_iterator left_, left_end_;
        right_iterator right_, right_end_;
        function_type fun_;
        value_type value_;
        void update() { if (left_ != left_end_ && right_ != right_end_) value_ = fun_(*left_, *right_); }
    };

    template <typename Range1, typename Range2, typename BinaryFunction>
    TL::Irange<ZipIterator<Range1, Range2, BinaryFunction>> operator>>(const Range1& lhs, ZipObject<Range2, BinaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        auto b1 = r.begin(), e1 = r.end();
        auto b2 = rhs.range.begin(), e2 = rhs.range.end();
        return {{b1, e1, b2, e2, rhs}, {e1, e1, e2, e2, rhs}};
    }

    template <typename Range2>
    inline ZipObject<Range2, void> zip(const Range2& r) {
        return {r};
    }

    template <typename Range2, typename BinaryFunction>
    inline ZipObject<Range2, BinaryFunction> zip(const Range2& r, BinaryFunction f) {
        return {r, f};
    }

}
