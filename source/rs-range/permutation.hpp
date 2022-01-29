#pragma once

#include "rs-range/core.hpp"
#include <algorithm>
#include <functional>
#include <iterator>

namespace RS::Range {

    // next_permutation, prev_permutation

    template <typename ComparisonPredicate, bool Next>
    struct PermutationObject:
    AlgorithmBase<PermutationObject<ComparisonPredicate, Next>> {
        ComparisonPredicate comp;
        PermutationObject() = default;
        PermutationObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> PermutationObject<CP2, Next> operator()(CP2 p) const { return p; }
        template <typename BidirectionalRange> void permute(BidirectionalRange& r) const {
            auto asr = Detail::as_range(r);
            if (Next)
                std::next_permutation(asr.begin(), asr.end(), comp);
            else
                std::prev_permutation(asr.begin(), asr.end(), comp);
        }
    };

    template <typename Range, typename ComparisonPredicate, bool Next>
    auto operator>>(const Range& lhs, PermutationObject<ComparisonPredicate, Next> rhs) {
        auto r = lhs >> collect;
        rhs.permute(r);
        return r;
    }

    template <typename BidirectionalRange, typename ComparisonPredicate, bool Next>
    BidirectionalRange& operator<<(BidirectionalRange& lhs, PermutationObject<ComparisonPredicate, Next> rhs) {
        rhs.permute(lhs);
        return lhs;
    }

    constexpr PermutationObject<std::less<>, true> next_permutation = {};
    constexpr PermutationObject<std::less<>, false> prev_permutation = {};

    // reverse

    struct ReverseObject:
    AlgorithmBase<ReverseObject> {};

    template <typename BidirectionalRange>
    TL::Irange<std::reverse_iterator<TL::RangeIterator<const BidirectionalRange>>>
    operator>>(const BidirectionalRange& lhs, ReverseObject /*rhs*/) {
        static_assert(Detail::category_is_at_least<BidirectionalRange, std::bidirectional_iterator_tag>);
        using reverse_iterator = std::reverse_iterator<TL::RangeIterator<const BidirectionalRange>>;
        auto r = Detail::as_range(lhs);
        reverse_iterator rb(r.end()), re(r.begin());
        return {rb, re};
    }

    template <typename BidirectionalRange>
    BidirectionalRange& operator<<(BidirectionalRange& lhs, ReverseObject /*rhs*/) {
        static_assert(Detail::category_is_at_least<BidirectionalRange, std::bidirectional_iterator_tag>);
        std::reverse(lhs.begin(), lhs.end());
        return lhs;
    }

    constexpr ReverseObject reverse = {};

    // shuffle

    template <typename RandomEngine>
    struct ShuffleObject:
    AlgorithmBase<ShuffleObject<RandomEngine>> {
        RandomEngine* rng;
        ShuffleObject(RandomEngine* r): rng(r) {}
    };

    template <typename Range, typename RandomEngine>
    auto operator>>(const Range& lhs, ShuffleObject<RandomEngine> rhs) {
        auto result = lhs >> collect;
        std::shuffle(result.begin(), result.end(), *rhs.rng);
        return result;
    }

    template <typename RandomAccessRange, typename RandomEngine>
    RandomAccessRange& operator<<(RandomAccessRange& lhs, ShuffleObject<RandomEngine> rhs) {
        std::shuffle(lhs.begin(), lhs.end(), *rhs.rng);
        return lhs;
    }

    template <typename RandomEngine>
    inline ShuffleObject<RandomEngine> shuffle(RandomEngine& rng) {
        return &rng;
    }

    // sort, stable_sort

    template <typename ComparisonPredicate, bool Stable>
    struct SortObject:
    AlgorithmBase<SortObject<ComparisonPredicate, Stable>> {
        ComparisonPredicate comp;
        SortObject() = default;
        SortObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> SortObject<CP2, Stable> operator()(CP2 p) const { return p; }
        template <typename RandomAccessRange> void sort(RandomAccessRange& r) const {
            auto asr = Detail::as_range(r);
            if (Stable)
                std::stable_sort(asr.begin(), asr.end(), comp);
            else
                std::sort(asr.begin(), asr.end(), comp);
        }
    };

    template <typename Range, typename ComparisonPredicate, bool Stable>
    auto operator>>(const Range& lhs, SortObject<ComparisonPredicate, Stable> rhs) {
        auto r = lhs >> collect;
        rhs.sort(r);
        return r;
    }

    template <typename RandomAccessRange, typename ComparisonPredicate, bool Stable>
    RandomAccessRange& operator<<(RandomAccessRange& lhs, SortObject<ComparisonPredicate, Stable> rhs) {
        rhs.sort(lhs);
        return lhs;
    }

    constexpr SortObject<std::less<>, false> sort = {};
    constexpr SortObject<std::less<>, true> stable_sort = {};

}
