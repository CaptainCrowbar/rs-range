#pragma once

#include "rs-tl/iterator.hpp"
#include "rs-tl/types.hpp"
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS::Range {

    namespace Detail {

        template <typename T> constexpr bool is_iterator_category =
            std::is_same_v<T, std::input_iterator_tag>
            || std::is_same_v<T, std::output_iterator_tag>
            || std::is_same_v<T, std::forward_iterator_tag>
            || std::is_same_v<T, std::bidirectional_iterator_tag>
            || std::is_same_v<T, std::random_access_iterator_tag>;

        template <typename Con, typename Val, typename = void> struct HasPushBack: std::false_type {};
        template <typename Con, typename Val>
            struct HasPushBack<Con, Val, std::void_t<decltype(std::declval<Con>().push_back(std::declval<const Val&>()))>>:
            std::true_type {};
        template <typename Con> constexpr bool has_push_back = HasPushBack<Con, TL::RangeValue<Con>>::value;

        template <typename Con, typename Val, typename = void> struct HasSimpleInsert: std::false_type {};
        template <typename Con, typename Val>
            struct HasSimpleInsert<Con, Val, std::void_t<decltype(std::declval<Con>().insert(std::declval<const Val&>()))>>:
            std::true_type {};
        template <typename Con> constexpr bool has_simple_insert = HasSimpleInsert<Con, TL::RangeValue<Con>>::value;

        template <typename T, int Path = is_iterator_category<T> ? 1 : TL::is_iterator<T> ? 2 : TL::is_range<T> ? 3 : 0>
            struct IteratorCategoryType;
        template <typename T> struct IteratorCategoryType<T, 1> { using type = T; };
        template <typename T> struct IteratorCategoryType<T, 2> { using type = typename std::iterator_traits<T>::iterator_category; };
        template <typename T> struct IteratorCategoryType<T, 3>
            { using type = typename std::iterator_traits<TL::RangeIterator<T>>::iterator_category; };
        template <typename T> using IteratorCategory = typename IteratorCategoryType<T>::type;

        template <typename Cat1, typename Cat2> struct CategoryLess { static constexpr bool value = ! CategoryLess<Cat2, Cat1>::value; };
        template <typename Cat> struct CategoryLess<Cat, Cat>: std::false_type {};
        template <> struct CategoryLess<std::input_iterator_tag, std::forward_iterator_tag>: std::true_type {};
        template <> struct CategoryLess<std::input_iterator_tag, std::bidirectional_iterator_tag>: std::true_type {};
        template <> struct CategoryLess<std::input_iterator_tag, std::random_access_iterator_tag>: std::true_type {};
        template <> struct CategoryLess<std::forward_iterator_tag, std::bidirectional_iterator_tag>: std::true_type {};
        template <> struct CategoryLess<std::forward_iterator_tag, std::random_access_iterator_tag>: std::true_type {};
        template <> struct CategoryLess<std::bidirectional_iterator_tag, std::random_access_iterator_tag>: std::true_type {};

        template <typename T1, typename T2>
            constexpr bool category_equals = std::is_same_v<IteratorCategory<T1>, IteratorCategory<T2>>;
        template <typename T1, typename T2>
            constexpr bool category_is_at_least = ! CategoryLess<IteratorCategory<T1>, IteratorCategory<T2>>::value;

        template <typename... TS> struct CommonCategoryType;
        template <typename T, typename... TS> struct CommonCategoryType<T, TS...>:
            CommonCategoryType<T, typename CommonCategoryType<TS...>::type> {};
        template <typename T1, typename T2> struct CommonCategoryType<T1, T2> {
            using type = std::conditional_t<CategoryLess<IteratorCategory<T2>, IteratorCategory<T1>>::value,
                IteratorCategory<T2>, IteratorCategory<T1>>;
        };
        template <typename... TS> using CommonCategory = typename CommonCategoryType<TS...>::type;

        template <typename Range1, typename Range2, typename BinaryFunction> struct PairResultType
            { using type = std::invoke_result_t<BinaryFunction, TL::RangeValue<Range1>, TL::RangeValue<Range2>>; };
        template <typename Range1, typename Range2> struct PairResultType<Range1, Range2, void>
            { using type = std::pair<TL::RangeValue<Range1>, TL::RangeValue<Range2>>; };
        template <typename Range1, typename Range2, typename BinaryFunction>
            using PairResult = typename PairResultType<Range1, Range2, BinaryFunction>::type;

        template <typename T, typename CV>
        struct FlexibleIterator:
        TL::BidirectionalIterator<T, CV> {
            CV& operator[](ptrdiff_t i) const noexcept { T t = static_cast<const T&>(*this); t += i; return *t; }
            friend T& operator-=(T& lhs, ptrdiff_t rhs) { return lhs += - rhs; }
            friend T operator+(const T& lhs, ptrdiff_t rhs) { T t = lhs; t += rhs; return t; }
            friend T operator+(ptrdiff_t lhs, const T& rhs) { T t = rhs; t += lhs; return t; }
            friend T operator-(const T& lhs, ptrdiff_t rhs) { T t = lhs; t -= rhs; return t; }
            friend bool operator<(const T& lhs, const T& rhs) noexcept { return lhs - rhs < 0; }
            friend bool operator>(const T& lhs, const T& rhs) noexcept { return rhs < lhs; }
            friend bool operator<=(const T& lhs, const T& rhs) noexcept { return ! (rhs < lhs); }
            friend bool operator>=(const T& lhs, const T& rhs) noexcept { return ! (lhs < rhs); }
        };

        struct IdentityFunction {
            template <typename T> T& operator()(T& t) const noexcept { return t; }
            template <typename T> const T& operator()(const T& t) const noexcept { return t; }
        };

        template <typename T, typename U = std::decay_t<T>>
        class AsRange {
        public:
            explicit AsRange(T& t): ptr_(&t) {}
            auto begin() const { using std::begin; return begin(*ptr_); }
            auto end() const { using std::end; return end(*ptr_); }
        private:
            T* ptr_;
        };

        template <typename T, typename U>
        class AsRange<T, std::optional<U>> {
        public:
            explicit AsRange(T& t): ptr_(&t) {}
            auto begin() const { return ptr_->has_value() ? &**ptr_ : nullptr; }
            auto end() const { return ptr_->has_value() ? &**ptr_ + 1 : nullptr; }
        private:
            T* ptr_;
        };

        template <typename T>
        auto as_range(T& t) {
            return AsRange<T>(t);
        }

        template <typename T>
        struct SharedIterator:
        public TL::RandomAccessIterator<SharedIterator<T>, T> {
            std::shared_ptr<std::vector<T>> share;
            typename std::vector<T>::iterator iter;
            T& operator*() const noexcept { return *iter; }
            SharedIterator& operator+=(ptrdiff_t rhs) noexcept { iter += rhs; return *this; }
            ptrdiff_t operator-(const SharedIterator& rhs) const noexcept { return iter - rhs.iter; }
        };

        template <typename T> using SharedRange = TL::Irange<SharedIterator<T>>;

        template <typename T, typename... Args>
        SharedRange<T> make_shared_range(Args&&... args) {
            SharedIterator<T> i, j;
            i.share = j.share = std::make_shared<std::vector<T>>(std::forward<Args>(args)...);
            i.iter = i.share->begin();
            j.iter = j.share->end();
            return {i, j};
        }

        template <typename Container, typename T>
        void append_to(Container& con, const T& t) {
            if constexpr (Detail::has_push_back<Container>)
                con.push_back(t);
            else if constexpr (Detail::has_simple_insert<Container>)
                con.insert(t);
            else
                con.insert(con.end(), t);
        }

    }

    static constexpr size_t npos = std::string::npos;

    using TL::append;
    using TL::overwrite;

    template <typename Range, typename Container>
    const Range& operator>>(const Range& lhs, TL::AppendIterator<Container> rhs) {
        auto r = Detail::as_range(lhs);
        std::copy(r.begin(), r.end(), rhs);
        return lhs;
    }

    // Algorithm combinators

    template <typename T>
    struct AlgorithmBase {};

    template <typename T1, typename T2>
    struct AlgorithmCombinator:
    AlgorithmBase<AlgorithmCombinator<T1, T2>> {
        T1 first;
        T2 second;
        AlgorithmCombinator(const T1& t1, const T2& t2): first(t1), second(t2) {}
    };

    template <typename T1, typename T2>
    AlgorithmCombinator<T1, T2> operator*(const AlgorithmBase<T1>& lhs, const AlgorithmBase<T2>& rhs) {
        return {*static_cast<const T1*>(&lhs), *static_cast<const T2*>(&rhs)};
    }

    template <typename Range, typename T1, typename T2>
    auto operator>>(const Range& lhs, const AlgorithmCombinator<T1, T2>& rhs) {
        return lhs >> rhs.first >> rhs.second;
    }

    template <typename Range, typename T1, typename T2>
    Range& operator<<(Range& lhs, const AlgorithmCombinator<T1, T2>& rhs) {
        return lhs << rhs.first << rhs.second;
    }

    // Output iterator wrapper

    template <typename OutputIterator>
    struct OutputObject:
    AlgorithmBase<OutputObject<OutputIterator>> {
        OutputIterator iter;
        OutputObject(OutputIterator i): iter(i) {}
    };

    template <typename Range, typename OutputIterator>
    void operator>>(const Range& lhs, OutputObject<OutputIterator> rhs) {
        auto r = Detail::as_range(lhs);
        std::copy(r.begin(), r.end(), rhs.iter);
    }

    template <typename OutputIterator>
    inline OutputObject<OutputIterator> output(OutputIterator i) {
        return i;
    }

    // Trivial passthrough filter

    struct PassthroughObject:
    AlgorithmBase<PassthroughObject> {};

    template <typename Range>
    class PassthroughIterator:
    public Detail::FlexibleIterator<PassthroughIterator<Range>, const TL::RangeValue<Range>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        PassthroughIterator() = default;
        PassthroughIterator(underlying_iterator i): iter_(i) {}
        const auto& operator*() const noexcept { return *iter_; }
        PassthroughIterator& operator++() { ++iter_; return *this; }
        PassthroughIterator& operator--() { --iter_; return *this; }
        PassthroughIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; return *this; }
        ptrdiff_t operator-(const PassthroughIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const PassthroughIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
    };

    template <typename Range>
    TL::Irange<PassthroughIterator<Range>> operator>>(const Range& lhs, PassthroughObject /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    template <typename Range>
    Range& operator<<(Range& lhs, PassthroughObject /*rhs*/) {
        return lhs;
    }

    constexpr PassthroughObject passthrough = {};

    // Collect a range as a container

    struct CollectObject:
    AlgorithmBase<CollectObject> {};

    template <typename Range>
    auto operator>>(const Range& lhs, CollectObject /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return Detail::make_shared_range<TL::RangeValue<Range>>(r.begin(), r.end());
    }

    constexpr CollectObject collect = {};

    template <typename Container>
    struct CollectAsObject:
    AlgorithmBase<CollectAsObject<Container>> {};

    template <typename Range, typename Container>
    auto operator>>(const Range& lhs, CollectAsObject<Container> /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return Container(r.begin(), r.end());
    }

    template <typename Container>
    inline CollectAsObject<Container> collect_as = {};

    // Simple iteration

    template <typename UnaryFunction>
    struct EachObject:
    AlgorithmBase<EachObject<UnaryFunction>> {
        UnaryFunction fun;
        EachObject(const UnaryFunction& f): fun(f) {}
    };

    template <typename Range, typename UnaryFunction>
    const Range& operator>>(const Range& lhs, EachObject<UnaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        std::for_each(r.begin(), r.end(), rhs.fun);
        return lhs;
    }

    template <typename Range, typename UnaryFunction>
    Range& operator<<(Range& lhs, EachObject<UnaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        std::for_each(r.begin(), r.end(), rhs.fun);
        return lhs;
    }

    template <typename UnaryFunction>
    inline EachObject<UnaryFunction> each(UnaryFunction f) {
        return f;
    }

    template <typename BinaryFunction>
    inline auto each_pair(BinaryFunction f) {
        return each([f] (auto& pair) { return f(pair.first, pair.second); });
    }

}
