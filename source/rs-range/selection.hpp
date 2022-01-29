#pragma once

#include "rs-range/core.hpp"
#include "rs-tl/iterator.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <random>
#include <utility>

namespace RS::Range {

    // before, until, from, after

    template <typename Predicate, bool Inclusive>
    struct BeforeObject:
    AlgorithmBase<BeforeObject<Predicate, Inclusive>> {
        Predicate pred;
        BeforeObject(const Predicate& p): pred(p) {}
    };

    template <typename Predicate, bool Inclusive>
    struct AfterObject:
    AlgorithmBase<AfterObject<Predicate, Inclusive>> {
        Predicate pred;
        AfterObject(const Predicate& p): pred(p) {}
    };

    template <typename Range, typename Predicate, bool Inclusive>
    class BeforeIterator:
    public TL::ForwardIterator<BeforeIterator<Range, Predicate, Inclusive>, const TL::RangeValue<Range>> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::RangeValue<Range>;
        using predicate_type = std::function<bool(const value_type&)>;
        BeforeIterator() = default;
        BeforeIterator(underlying_iterator i, underlying_iterator e, Predicate p): iter_(i), end_(e), pred_(p) {
            if constexpr (! Inclusive)
                if (i != e && p(*i))
                    iter_ = e;
        }
        const auto& operator*() const noexcept { return *iter_; }
        BeforeIterator& operator++() {
            if constexpr (Inclusive) {
                if (pred_(*iter_))
                    iter_ = end_;
                else
                    ++iter_;
            } else {
                ++iter_;
                if (iter_ != end_ && pred_(*iter_))
                    iter_ = end_;
            }
            return *this;
        }
        bool operator==(const BeforeIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        underlying_iterator end_;
        predicate_type pred_;
    };

    template <typename Range, typename Predicate, bool Inclusive>
    TL::Irange<BeforeIterator<Range, Predicate, Inclusive>> operator>>(const Range& lhs, BeforeObject<Predicate, Inclusive> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.pred}, {e, e, {rhs.pred}}};
    }

    template <typename Range, typename Predicate, bool Inclusive>
    TL::Irange<TL::RangeIterator<const Range>> operator>>(const Range& lhs, AfterObject<Predicate, Inclusive> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        auto i = std::find_if(b, e, rhs.pred);
        if constexpr (! Inclusive)
            if (i != e)
                ++i;
        return {i, e};
    }

    template <typename Container, typename Predicate, bool Inclusive>
    Container& operator<<(Container& lhs, BeforeObject<Predicate, Inclusive> rhs) {
        auto b = lhs.begin(), e = lhs.end();
        auto i = std::find_if(b, e, rhs.pred);
        if constexpr (Inclusive)
            if (i != e)
                ++i;
        Container temp;
        std::copy(b, i, append(temp));
        lhs = std::move(temp);
        return lhs;
    }

    template <typename Container, typename Predicate, bool Inclusive>
    Container& operator<<(Container& lhs, AfterObject<Predicate, Inclusive> rhs) {
        auto b = lhs.begin(), e = lhs.end();
        auto i = std::find_if(b, e, rhs.pred);
        if constexpr (! Inclusive)
            if (i != e)
                ++i;
        Container temp;
        std::copy(i, e, append(temp));
        lhs = std::move(temp);
        return lhs;
    }

    template <typename Predicate>
    inline BeforeObject<Predicate, false> before_if(Predicate p) {
        return p;
    }

    template <typename Predicate>
    inline BeforeObject<Predicate, true> until_if(Predicate p) {
        return p;
    }

    template <typename Predicate>
    inline AfterObject<Predicate, true> from_if(Predicate p) {
        return p;
    }

    template <typename Predicate>
    inline AfterObject<Predicate, false> after_if(Predicate p) {
        return p;
    }

    template <typename T>
    inline auto before(T t) {
        return before_if([t] (const auto& x) { return x == t; });
    }

    template <typename T>
    inline auto until(T t) {
        return until_if([t] (const auto& x) { return x == t; });
    }

    template <typename T>
    inline auto from(T t) {
        return from_if([t] (const auto& x) { return x == t; });
    }

    template <typename T>
    inline auto after(T t) {
        return after_if([t] (const auto& x) { return x == t; });
    }

    // filter, filter_out

    template <typename Predicate>
    struct FilterObject:
    AlgorithmBase<FilterObject<Predicate>> {
        Predicate pred;
        bool sense;
        FilterObject(const Predicate& p, bool s): pred(p), sense(s) {}
    };

    template <typename Range, typename Predicate>
    class FilterIterator:
    public TL::ForwardIterator<FilterIterator<Range, Predicate>, const TL::RangeValue<Range>> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::RangeValue<Range>;
        using predicate_type = std::function<bool(const value_type&)>;
        FilterIterator() = default;
        FilterIterator(underlying_iterator i, underlying_iterator e, Predicate p, bool s):
            iter_(i), end_(e), pred_(p), sense_(s) { update(); }
        const auto& operator*() const noexcept { return *iter_; }
        FilterIterator& operator++() { ++iter_; update(); return *this; }
        bool operator==(const FilterIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        underlying_iterator end_;
        predicate_type pred_;
        bool sense_;
        void update() { while (iter_ != end_ && pred_(*iter_) != sense_) ++iter_; }
    };

    template <typename Range, typename Predicate>
    TL::Irange<FilterIterator<Range, Predicate>> operator>>(const Range& lhs, FilterObject<Predicate> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.pred, rhs.sense}, {e, e, rhs.pred, rhs.sense}};
    }

    template <typename Container, typename Predicate>
    Container& operator<<(Container& lhs, FilterObject<Predicate> rhs) {
        lhs.erase(std::remove_if(lhs.begin(), lhs.end(), [=] (auto& x) { return rhs.pred(x) != rhs.sense; }), lhs.end());
        return lhs;
    }

    template <typename Predicate>
    inline FilterObject<Predicate> filter(Predicate p) {
        return {p, true};
    }

    template <typename Predicate>
    inline FilterObject<Predicate> filter_out(Predicate p) {
        return {p, false};
    }

    // not_null

    struct NotNullObject:
    AlgorithmBase<NotNullObject> {};

    template <typename Range>
    class NotNullIterator:
    public TL::ForwardIterator<NotNullIterator<Range>, const TL::RangeValue<Range>> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::RangeValue<Range>;
        NotNullIterator() = default;
        NotNullIterator(underlying_iterator i, underlying_iterator e): iter_(i), end_(e) { update(); }
        const auto& operator*() const noexcept { return *iter_; }
        NotNullIterator& operator++() { ++iter_; update(); return *this; }
        bool operator==(const NotNullIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        underlying_iterator end_;
        void update() { while (iter_ != end_ && ! *iter_) ++iter_; }
    };

    template <typename Range>
    TL::Irange<NotNullIterator<Range>> operator>>(const Range& lhs, NotNullObject) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e}, {e, e}};
    }

    template <typename Container>
    Container& operator<<(Container& lhs, NotNullObject) {
        lhs.erase(std::remove_if(lhs.begin(), lhs.end(), [=] (auto& x) { return ! bool(x); }), lhs.end());
        return lhs;
    }

    constexpr NotNullObject not_null = {};

    // remove

    template <typename T>
    struct RemoveObject:
    AlgorithmBase<RemoveObject<T>> {
        T value;
        RemoveObject(const T& t): value(t) {}
    };

    template <typename Range, typename T>
    class RemoveIterator:
    public TL::ForwardIterator<RemoveIterator<Range, T>, const TL::RangeValue<Range>> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::RangeValue<Range>;
        using predicate_type = std::function<bool(const value_type&)>;
        RemoveIterator() = default;
        RemoveIterator(underlying_iterator i, underlying_iterator e, const T& t): iter_(i), end_(e), value_(t) { update(); }
        const auto& operator*() const noexcept { return *iter_; }
        RemoveIterator& operator++() { ++iter_; update(); return *this; }
        bool operator==(const RemoveIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        underlying_iterator end_;
        T value_;
        void update() { while (iter_ != end_ && *iter_ == value_) ++iter_; }
    };

    template <typename Range, typename T>
    TL::Irange<RemoveIterator<Range, T>> operator>>(const Range& lhs, RemoveObject<T> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.value}, {e, e, rhs.value}};
    }

    template <typename Container, typename T>
    Container& operator<<(Container& lhs, RemoveObject<T> rhs) {
        lhs.erase(std::remove(lhs.begin(), lhs.end(), rhs.value), lhs.end());
        return lhs;
    }

    template <typename T>
    inline RemoveObject<T> remove(const T& t) {
        return {t};
    }

    // sample_k

    template <typename RandomEngine>
    struct SampleByNumberObject:
    AlgorithmBase<SampleByNumberObject<RandomEngine>> {
        size_t num;
        RandomEngine* rng;
        SampleByNumberObject(size_t k, RandomEngine* r): num(k), rng(r) {}
    };

    template <typename RandomAccessRange, typename RandomEngine>
    auto operator>>(const RandomAccessRange& lhs, SampleByNumberObject<RandomEngine> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        size_t n = std::distance(b, e), m = std::min(rhs.num, n);
        auto result = Detail::make_shared_range<TL::RangeValue<RandomAccessRange>>(b, b + m);
        for (size_t i = m; i < n; ++i) {
            std::uniform_int_distribution<size_t> dist(0, i - 1);
            auto j = dist(*rhs.rng);
            if (j < m)
                result.first[j] = b[i];
        }
        return result;
    }

    template <typename Container, typename RandomEngine>
    Container& operator<<(Container& lhs, SampleByNumberObject<RandomEngine> rhs) {
        auto sample_k = lhs >> rhs;
        Container temp(sample_k.begin(), sample_k.end());
        lhs = std::move(temp);
        return lhs;
    }

    template <typename RandomEngine>
    inline SampleByNumberObject<RandomEngine> sample_k(size_t k, RandomEngine& rng) {
        return {k, &rng};
    }

    // sample_p

    template <typename RandomEngine>
    struct SampleByProbObject:
    AlgorithmBase<SampleByProbObject<RandomEngine>> {
        double prob;
        RandomEngine* rng;
        SampleByProbObject(double p, RandomEngine* r): prob(p), rng(r) {}
    };

    template <typename Range, typename RandomEngine>
    class SampleByProbIterator:
    public TL::ForwardIterator<SampleByProbIterator<Range, RandomEngine>, const TL::RangeValue<Range>> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::RangeValue<Range>;
        SampleByProbIterator() = default;
        SampleByProbIterator(underlying_iterator i, underlying_iterator e, double p, RandomEngine* r):
            iter_(i), end_(e), dist_(p), rng_(r) { update(); }
        const auto& operator*() const noexcept { return *iter_; }
        SampleByProbIterator& operator++() { ++iter_; update(); return *this; }
        bool operator==(const SampleByProbIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        underlying_iterator end_;
        std::bernoulli_distribution dist_;
        double prob = 0;
        RandomEngine* rng_ = nullptr;
        void update() { while (iter_ != end_ && ! dist_(*rng_)) ++iter_; }
    };

    template <typename Range, typename RandomEngine>
    TL::Irange<SampleByProbIterator<Range, RandomEngine>> operator>>(const Range& lhs, SampleByProbObject<RandomEngine> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.prob, rhs.rng}, {e, e, 0, nullptr}};
    }

    template <typename Container, typename RandomEngine>
    Container& operator<<(Container& lhs, SampleByProbObject<RandomEngine> rhs) {
        Container temp;
        std::bernoulli_distribution dist(rhs.prob);
        std::copy_if(lhs.begin(), lhs.end(), append(temp), [&] (auto&) { return dist(*rhs.rng); });
        lhs = std::move(temp);
        return lhs;
    }

    template <typename RandomEngine>
    inline SampleByProbObject<RandomEngine> sample_p(double p, RandomEngine& rng) {
        return {p, &rng};
    }

    // sample_replace

    template <typename RandomEngine>
    struct SampleReplaceObject:
    AlgorithmBase<SampleReplaceObject<RandomEngine>> {
        size_t num;
        RandomEngine* rng;
        SampleReplaceObject(size_t k, RandomEngine* r): num(k), rng(r) {}
    };

    template <typename RandomAccessRange, typename RandomEngine>
    auto operator>>(const RandomAccessRange& lhs, SampleReplaceObject<RandomEngine> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        size_t n = std::distance(b, e);
        auto result = Detail::make_shared_range<TL::RangeValue<RandomAccessRange>>(rhs.num);
        std::uniform_int_distribution<size_t> dist(0, n - 1);
        for (auto& x: result)
            x = b[dist(*rhs.rng)];
        return result;
    }

    template <typename Container, typename RandomEngine>
    Container& operator<<(Container& lhs, SampleReplaceObject<RandomEngine> rhs) {
        auto sample = lhs >> rhs;
        Container temp(sample.begin(), sample.end());
        lhs = std::move(temp);
        return lhs;
    }

    template <typename RandomEngine>
    inline SampleReplaceObject<RandomEngine> sample_replace(size_t k, RandomEngine& rng) {
        return {k, &rng};
    }

    // skip, take

    struct SkipObject:
    AlgorithmBase<SkipObject> {
        size_t num;
    };

    struct TakeObject:
    AlgorithmBase<TakeObject> {
        size_t num;
    };

    template <typename Range>
    class TakeIterator:
    public TL::ForwardIterator<TakeIterator<Range>, const TL::RangeValue<Range>> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::RangeValue<Range>;
        TakeIterator() = default;
        TakeIterator(underlying_iterator i, underlying_iterator e, size_t n):
            iter_(i), end_(e), num_(n) { if (n == 0) iter_ = end_; }
        const auto& operator*() const noexcept { return *iter_; }
        TakeIterator& operator++() { if (--num_ == 0) iter_ = end_; else ++iter_; return *this; }
        bool operator==(const TakeIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        underlying_iterator end_;
        size_t num_;
    };

    template <typename Range, bool RA = Detail::category_equals<Range, std::random_access_iterator_tag>>
    struct SkipToIndex;

    template <typename Range>
    struct SkipToIndex<Range, true> {
        TL::RangeIterator<Range> operator()(Range& r, size_t n) const {
            auto asr = Detail::as_range(r);
            auto b = asr.begin(), e = asr.end();
            n = std::min(n, size_t(std::distance(b, e)));
            std::advance(b, n);
            return b;
        }
    };

    template <typename Range>
    struct SkipToIndex<Range, false> {
        TL::RangeIterator<Range> operator()(Range& r, size_t n) const {
            auto asr = Detail::as_range(r);
            auto b = asr.begin(), e = asr.end();
            for (; n > 0 && b != e; ++b, --n) {}
            return b;
        }
    };

    template <typename Range>
    TL::Irange<TL::RangeIterator<const Range>> operator>>(const Range& lhs, SkipObject rhs) {
        auto r = Detail::as_range(lhs);
        return {SkipToIndex<const Range>()(lhs, rhs.num), r.end()};
    }

    template <typename Range>
    TL::Irange<TakeIterator<Range>> operator>>(const Range& lhs, TakeObject rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.num}, {e, e, 0}};
    }

    template <typename Container>
    Container& operator<<(Container& lhs, SkipObject rhs) {
        auto i = SkipToIndex<Container>()(lhs, rhs.num);
        lhs.erase(lhs.begin(), i);
        return lhs;
    }

    template <typename Container>
    Container& operator<<(Container& lhs, TakeObject rhs) {
        auto i = SkipToIndex<Container>()(lhs, rhs.num);
        lhs.erase(i, lhs.end());
        return lhs;
    }

    inline SkipObject skip(size_t n) {
        SkipObject o;
        o.num = n;
        return o;
    }

    inline TakeObject take(size_t n) {
        TakeObject o;
        o.num = n;
        return o;
    }

    // stride

    struct StrideObject:
    AlgorithmBase<StrideObject> {
        size_t skip;
        size_t step;
        StrideObject(size_t a, size_t b): skip(a), step(b) {}
    };

    template <typename Range>
    class StrideIterator:
    public TL::ForwardIterator<StrideIterator<Range>, const TL::RangeValue<Range>> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::RangeValue<Range>;
        StrideIterator() = default;
        StrideIterator(underlying_iterator i, underlying_iterator e, size_t skip, size_t step):
            iter_(i), end_(e), delta_(step) { advance_by(skip); }
        const auto& operator*() const noexcept { return *iter_; }
        StrideIterator& operator++() { advance_by(delta_); return *this; }
        bool operator==(const StrideIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        underlying_iterator end_;
        size_t delta_;
        void advance_by(size_t n) {
            if constexpr (Detail::category_equals<underlying_iterator, std::random_access_iterator_tag>) {
                n = std::min(n, size_t(std::distance(iter_, end_)));
                std::advance(iter_, n);
            } else {
                for (; iter_ != end_ && n > 0; ++iter_, --n) {}
            }
        }
    };

    template <typename Range>
    TL::Irange<StrideIterator<Range>> operator>>(const Range& lhs, StrideObject rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.skip, rhs.step}, {e, e, 0, 0}};
    }

    template <typename Container>
    Container& operator<<(Container& lhs, StrideObject rhs) {
        Container temp;
        lhs >> rhs >> append(temp);
        lhs = std::move(temp);
        return lhs;
    }

    inline StrideObject stride(size_t step) {
        return {step - 1, step};
    }

    inline StrideObject stride(size_t skip, size_t step) {
        return {skip, step};
    }

    // unique

    template <typename EqualityPredicate>
    struct UniqueObject:
    AlgorithmBase<UniqueObject<EqualityPredicate>> {
        EqualityPredicate eq;
        UniqueObject() = default;
        UniqueObject(const EqualityPredicate& p): eq(p) {}
        template <typename EP2> UniqueObject<EP2> operator()(EP2 p) const { return p; }
    };

    template <typename Range, typename EqualityPredicate>
    class UniqueIterator:
    public TL::ForwardIterator<UniqueIterator<Range, EqualityPredicate>, const TL::RangeValue<Range>> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = TL::RangeValue<Range>;
        using equality_type = std::function<bool(const value_type&, const value_type&)>;
        UniqueIterator() = default;
        UniqueIterator(underlying_iterator i, underlying_iterator e, EqualityPredicate p): iter_(i), end_(e), eq_(p) {}
        auto& operator*() const noexcept { return *iter_; }
        UniqueIterator& operator++() { auto j = iter_; do ++iter_; while (iter_ != end_ && eq_(*j, *iter_)); return *this; }
        bool operator==(const UniqueIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        underlying_iterator end_;
        equality_type eq_;
    };

    template <typename Range, typename EqualityPredicate>
    TL::Irange<UniqueIterator<Range, EqualityPredicate>> operator>>(const Range& lhs, UniqueObject<EqualityPredicate> rhs) {
        auto r = Detail::as_range(lhs);
        auto b = r.begin(), e = r.end();
        return {{b, e, rhs.eq}, {e, e, rhs.eq}};
    }

    template <typename Container, typename EqualityPredicate>
    Container& operator<<(Container& lhs, UniqueObject<EqualityPredicate> rhs) {
        Container temp;
        std::unique_copy(lhs.begin(), lhs.end(), append(temp), rhs.eq);
        lhs = std::move(temp);
        return lhs;
    }

    constexpr UniqueObject<std::equal_to<>> unique = {};

}
