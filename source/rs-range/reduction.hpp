#pragma once

#include "rs-range/core.hpp"
#include "rs-range/permutation.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <utility>

namespace RS::Range {

    // all_of, any_of, none_of

    template <typename Predicate>
    struct AllOfObject:
    AlgorithmBase<AllOfObject<Predicate>> {
        Predicate pred;
        AllOfObject(const Predicate& p): pred(p) {}
    };

    template <typename Predicate>
    struct AnyOfObject:
    AlgorithmBase<AnyOfObject<Predicate>> {
        Predicate pred;
        AnyOfObject(const Predicate& p): pred(p) {}
    };

    template <typename Predicate>
    struct NoneOfObject:
    AlgorithmBase<NoneOfObject<Predicate>> {
        Predicate pred;
        NoneOfObject(const Predicate& p): pred(p) {}
    };

    template <typename Range, typename Predicate>
    size_t operator>>(const Range& lhs, const AllOfObject<Predicate>& rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return std::find_if_not(b, e, rhs.pred) == e;
    }

    template <typename Range, typename Predicate>
    size_t operator>>(const Range& lhs, const AnyOfObject<Predicate>& rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return std::find_if(b, e, rhs.pred) != e;
    }

    template <typename Range, typename Predicate>
    size_t operator>>(const Range& lhs, const NoneOfObject<Predicate>& rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return std::find_if(b, e, rhs.pred) == e;
    }

    template <typename Predicate>
    inline AllOfObject<Predicate> all_of(Predicate p) {
        return p;
    }

    template <typename Predicate>
    inline AnyOfObject<Predicate> any_of(Predicate p) {
        return p;
    }

    template <typename Predicate>
    inline NoneOfObject<Predicate> none_of(Predicate p) {
        return p;
    }

    // count, count_if

    template <typename T>
    struct CountObject:
    AlgorithmBase<CountObject<T>> {
        T value;
        CountObject(const T& t): value(t) {}
    };

    template <>
    struct CountObject<void>:
    AlgorithmBase<CountObject<void>> {
        template <typename T> CountObject<T> operator()(const T& t) const { return t; }
    };

    template <typename Predicate>
    struct CountIfObject:
    AlgorithmBase<CountIfObject<Predicate>> {
        Predicate pred;
        CountIfObject(const Predicate& p): pred(p) {}
    };

    template <typename Range>
    size_t operator>>(const Range& lhs, CountObject<void> /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return std::distance(r.begin(), r.end());
    }

    template <typename Range, typename T>
    size_t operator>>(const Range& lhs, const CountObject<T>& rhs) {
        auto r = Detail::as_range(lhs);
        return std::count(r.begin(), r.end(), rhs.value);
    }

    template <typename Range, typename Predicate>
    size_t operator>>(const Range& lhs, const CountIfObject<Predicate>& rhs) {
        auto r = Detail::as_range(lhs);
        return std::count_if(r.begin(), r.end(), rhs.pred);
    }

    constexpr CountObject<void> count = {};

    template <typename Predicate>
    inline CountIfObject<Predicate> count_if(Predicate p) {
        return p;
    }

    // fold_left, fold_right

    template <typename T, typename BinaryFunction>
    struct FoldLeftObject:
    AlgorithmBase<FoldLeftObject<T, BinaryFunction>> {
        T init;
        BinaryFunction fun;
        FoldLeftObject(const T& t, const BinaryFunction& f): init(t), fun(f) {}
    };

    template <typename T, typename BinaryFunction>
    struct FoldRightObject:
    AlgorithmBase<FoldRightObject<T, BinaryFunction>> {
        T init;
        BinaryFunction fun;
        FoldRightObject(const T& t, const BinaryFunction& f): init(t), fun(f) {}
    };

    template <typename Range, typename T, typename BinaryFunction>
    T operator>>(const Range& lhs, const FoldLeftObject<T, BinaryFunction>& rhs) {
        auto r = Detail::as_range(lhs);
        return std::accumulate(r.begin(), r.end(), rhs.init, rhs.fun);
    }

    template <typename BidirectionalRange, typename T, typename BinaryFunction>
    T operator>>(const BidirectionalRange& lhs, const FoldRightObject<T, BinaryFunction>& rhs) {
        static_assert(Detail::category_is_at_least<BidirectionalRange, std::bidirectional_iterator_tag>);
        auto rev = lhs >> reverse;
        auto asrev = Detail::as_range(rev);
        auto f = rhs.fun;
        auto fyx = [f] (const auto& x, const auto& y) { return f(y, x); };
        return std::accumulate(asrev.begin(), asrev.end(), rhs.init, fyx);
    }

    template <typename T, typename BinaryFunction>
    inline FoldLeftObject<T, BinaryFunction> fold_left(const T& t, BinaryFunction f) {
        return {t, f};
    }

    template <typename T, typename BinaryFunction>
    inline FoldRightObject<T, BinaryFunction> fold_right(const T& t, BinaryFunction f) {
        return {t, f};
    }

    // is_empty, is_nonempty

    template <bool Sense>
    struct IsEmptyObject:
    AlgorithmBase<IsEmptyObject<Sense>> {};

    template <typename Range, bool Sense>
    bool operator>>(const Range& lhs, IsEmptyObject<Sense>) {
        auto r = Detail::as_range(lhs);
        return (r.begin() == r.end()) == Sense;
    }

    constexpr IsEmptyObject<true> is_empty = {};
    constexpr IsEmptyObject<false> is_nonempty = {};

    // is_sorted

    template <typename ComparisonPredicate>
    struct IsSortedObject:
    AlgorithmBase<IsSortedObject<ComparisonPredicate>> {
        ComparisonPredicate comp;
        IsSortedObject() = default;
        IsSortedObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> IsSortedObject<CP2> operator()(CP2 p) const { return {p}; }
    };

    template <typename Range, typename ComparisonPredicate>
    bool operator>>(const Range& lhs, const IsSortedObject<ComparisonPredicate>& rhs) {
        auto r = Detail::as_range(lhs);
        auto i = r.begin(), j = i, k = r.end();
        if (i == k || ++j == k)
            return true;
        for (; j != k; ++i, ++j)
            if (rhs.comp(*j, *i))
                return false;
        return true;
    }

    constexpr IsSortedObject<std::less<>> is_sorted = {};

    // min, max, min_max

    template <typename ComparisonPredicate>
    struct MinObject:
    AlgorithmBase<MinObject<ComparisonPredicate>> {
        ComparisonPredicate comp;
        MinObject() = default;
        MinObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> MinObject<CP2> operator()(CP2 p) const { return {p}; }
    };

    template <typename ComparisonPredicate>
    struct MaxObject:
    AlgorithmBase<MaxObject<ComparisonPredicate>> {
        ComparisonPredicate comp;
        MaxObject() = default;
        MaxObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> MaxObject<CP2> operator()(CP2 p) const { return {p}; }
    };

    template <typename ComparisonPredicate>
    struct MinMaxObject:
    AlgorithmBase<MinMaxObject<ComparisonPredicate>> {
        ComparisonPredicate comp;
        MinMaxObject() = default;
        MinMaxObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> MinMaxObject<CP2> operator()(CP2 p) const { return {p}; }
    };

    template <typename Range, typename ComparisonPredicate>
    TL::RangeValue<Range> operator>>(const Range& lhs, const MinObject<ComparisonPredicate>& rhs) {
        auto r = Detail::as_range(lhs);
        auto i = r.begin(), j = r.end();
        if (i == j)
            return {};
        else
            return *std::min_element(i, j, rhs.comp);
    }

    template <typename Range, typename ComparisonPredicate>
    TL::RangeValue<Range> operator>>(const Range& lhs, const MaxObject<ComparisonPredicate>& rhs) {
        auto r = Detail::as_range(lhs);
        auto i = r.begin(), j = r.end();
        if (i == j)
            return {};
        else
            return *std::max_element(i, j, rhs.comp);
    }

    template <typename Range, typename ComparisonPredicate>
    std::pair<TL::RangeValue<Range>, TL::RangeValue<Range>>
    operator>>(const Range& lhs, const MinMaxObject<ComparisonPredicate>& rhs) {
        auto r = Detail::as_range(lhs);
        auto i = r.begin(), j = r.end();
        if (i == j)
            return {};
        auto ipair = std::minmax_element(i, j, rhs.comp);
        return {*ipair.first, *ipair.second};
    }

    constexpr MinObject<std::less<>> min = {};
    constexpr MaxObject<std::less<>> max = {};
    constexpr MinMaxObject<std::less<>> min_max = {};

    // reduce

    template <typename BinaryFunction>
    struct ReduceObject:
    AlgorithmBase<ReduceObject<BinaryFunction>> {
        BinaryFunction fun;
        ReduceObject(const BinaryFunction& f): fun(f) {}
    };

    template <typename Range, typename BinaryFunction>
    TL::RangeValue<Range> operator>>(const Range& lhs, const ReduceObject<BinaryFunction>& rhs) {
        auto r = Detail::as_range(lhs);
        auto i = r.begin(), j = r.end();
        if (i == j)
            return {};
        auto t = *i++;
        return std::accumulate(i, j, t, rhs.fun);
    }

    template <typename BinaryFunction>
    inline ReduceObject<BinaryFunction> reduce(BinaryFunction f) {
        return {f};
    }

    // sum, product

    template <typename T>
    struct SumObject:
    AlgorithmBase<SumObject<T>> {
        T init;
        SumObject(const T& t): init(t) {}
    };

    template <>
    struct SumObject<void>:
    AlgorithmBase<SumObject<void>> {
        template <typename T> SumObject<T> operator()(const T& t) const { return {t}; }
    };

    template <typename T>
    struct ProductObject:
    AlgorithmBase<ProductObject<T>> {
        T init;
        ProductObject(const T& t): init(t) {}
    };

    template <>
    struct ProductObject<void>:
    AlgorithmBase<ProductObject<void>> {
        template <typename T> ProductObject<T> operator()(const T& t) const { return {t}; }
    };

    template <typename Range, typename T>
    T operator>>(const Range& lhs, const SumObject<T>& rhs) {
        auto r = Detail::as_range(lhs);
        return std::accumulate(r.begin(), r.end(), rhs.init);
    }

    template <typename Range>
    TL::RangeValue<Range> operator>>(const Range& lhs, const SumObject<void>& /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return std::accumulate(r.begin(), r.end(), TL::RangeValue<Range>());
    }

    template <typename Range, typename T>
    T operator>>(const Range& lhs, const ProductObject<T>& rhs) {
        auto r = Detail::as_range(lhs);
        return std::accumulate(r.begin(), r.end(), rhs.init, std::multiplies<>());
    }

    template <typename Range>
    TL::RangeValue<Range> operator>>(const Range& lhs, const ProductObject<void>& /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return std::accumulate(r.begin(), r.end(), TL::RangeValue<Range>(1), std::multiplies<>());
    }

    constexpr SumObject<void> sum = {};
    constexpr ProductObject<void> product = {};

}
