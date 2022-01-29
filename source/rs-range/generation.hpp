#pragma once

#include "rs-range/core.hpp"
#include "rs-tl/iterator.hpp"
#include <functional>
#include <type_traits>
#include <utility>

namespace RS::Range {

    // epsilon

    template <typename T>
    struct EpsilonObject {
        const T* begin() const { return nullptr; }
        const T* end() const { return nullptr; }
    };

    template <typename T>
    inline EpsilonObject<T> epsilon = {};

    // fill, single

    template <typename T>
    class FillIterator:
    public TL::RandomAccessIterator<FillIterator<T>, const T> {
    public:
        FillIterator() = default;
        FillIterator(const T& t, size_t n): value_(t), num_(n) {}
        const T& operator*() const noexcept { return value_; }
        FillIterator& operator+=(ptrdiff_t rhs) noexcept
            { if (rhs < 0) num_ += size_t(- rhs); else num_ -= size_t(rhs); return *this; }
        ptrdiff_t operator-(const FillIterator& rhs) const noexcept
            { return num_ <= rhs.num_ ? ptrdiff_t(rhs.num_ - num_) : - ptrdiff_t(num_ - rhs.num_); }
    private:
        T value_;
        size_t num_ = 0;
    };

    template <typename Range, typename T>
    Range& operator<<(Range& lhs, const TL::Irange<FillIterator<T>>& rhs) {
        auto i = rhs.begin();
        for (auto& x: lhs)
            x = *i++;
        return lhs;
    }

    template <typename T>
    inline TL::Irange<FillIterator<T>> fill(const T& t, size_t n = npos) {
        return {{t, n}, {}};
    }

    template <typename T>
    inline TL::Irange<FillIterator<T>> single(const T& t) {
        return {{t, 1}, {}};
    }

    // generate

    template <typename Function>
    class GenerateIterator:
    public TL::InputIterator<GenerateIterator<Function>, const std::decay_t<decltype(std::declval<Function>()())>> {
    public:
        using value_type = std::decay_t<decltype(std::declval<Function>()())>;
        using function_type = std::function<value_type()>;
        GenerateIterator() = default;
        GenerateIterator(Function f, size_t n): fun_(f), num_(n), ok_(false) {}
        const value_type& operator*() const noexcept { if (! ok_) { value = fun_(); ok_ = true; } return value; }
        GenerateIterator& operator++() { --num_; ok_ = false; return *this; }
        bool operator==(const GenerateIterator& rhs) const noexcept { return num_ == rhs.num_; }
    private:
        function_type fun_;
        size_t num_ = 0;
        mutable value_type value;
        mutable bool ok_ = false;
    };

    template <typename Range, typename Function>
    Range& operator<<(Range& lhs, const TL::Irange<GenerateIterator<Function>>& rhs) {
        auto i = rhs.begin();
        for (auto& x: lhs)
            x = *i++;
        return lhs;
    }

    template <typename Function>
    inline TL::Irange<GenerateIterator<Function>> generate(Function f, size_t n = npos) {
        return {{f, n}, {}};
    }

    // iota

    template <typename T>
    class IotaIterator:
    public TL::ForwardIterator<IotaIterator<T>, const T> {
    public:
        using function_type = std::function<T(const T&)>;
        IotaIterator() = default;
        template <typename UnaryFunction> IotaIterator(const T& t, UnaryFunction f, size_t n): value_(t), fun_(f), num_(n) {}
        const T& operator*() const noexcept { return value_; }
        IotaIterator& operator++() { value_ = fun_(value_); --num_; return *this; }
        bool operator==(const IotaIterator& rhs) const noexcept { return num_ == rhs.num_; }
    private:
        T value_;
        function_type fun_;
        size_t num_ = 0;
    };

    template <typename Range, typename T>
    Range& operator<<(Range& lhs, const TL::Irange<IotaIterator<T>>& rhs) {
        auto i = rhs.begin();
        for (auto& x: lhs)
            x = *i++;
        return lhs;
    }

    template <typename T, typename UnaryFunction>
    inline TL::Irange<IotaIterator<T>> iota(const T& init, UnaryFunction f, size_t n = npos) {
        return {{init, f, n}, {}};
    }

    template <typename T>
    inline auto iota(const T& init) {
        return iota(init, [] (T t) { return T(++t); });
    }

    template <typename T>
    inline auto iota(const T& init, const T& delta, size_t n = npos) {
        return iota(init, [delta] (const T& t) { return T(t + delta); }, n);
    }

    // random

    template <typename RandomDistributon, typename RandomEngine>
    class RandomIterator:
    public TL::InputIterator<RandomIterator<RandomDistributon, RandomEngine>, const typename RandomDistributon::result_type> {
    public:
        using value_type = typename RandomDistributon::result_type;
        RandomIterator() = default;
        RandomIterator(RandomDistributon d, RandomEngine& r, size_t n): dist_(d), rng_(&r), num_(n), value_(dist_(r)) {}
        const value_type& operator*() const noexcept { return value_; }
        RandomIterator& operator++() { --num_; value_ = dist_(*rng_); return *this; }
        bool operator==(const RandomIterator& rhs) const noexcept { return num_ == rhs.num_; }
    private:
        RandomDistributon dist_;
        RandomEngine* rng_;
        size_t num_ = 0;
        value_type value_;
    };

    template <typename Range, typename RandomDistributon, typename RandomEngine>
    Range& operator<<(Range& lhs, const TL::Irange<RandomIterator<RandomDistributon, RandomEngine>>& rhs) {
        auto i = rhs.begin();
        for (auto& x: lhs)
            x = *i++;
        return lhs;
    }

    template <typename RandomDistributon, typename RandomEngine>
    inline TL::Irange<RandomIterator<RandomDistributon, RandomEngine>>
    random(RandomDistributon d, RandomEngine& r, size_t n = npos) {
        return {{d, r, n}, {}};
    }

}
