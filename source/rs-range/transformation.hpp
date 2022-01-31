#pragma once

#include "rs-range/core.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace RS::Range {

    // const_ptr, dynamic_ptr, reinterpret_ptr, static_ptr

    namespace Detail {

        template <typename T2> struct GenericConstCast {
            template <typename T1> T2* operator()(T1* p) const { return const_cast<T2*>(p); }
            template <typename T1> std::shared_ptr<T2> operator()(const std::shared_ptr<T1>& p) const {
                return std::const_pointer_cast<T2>(p);
            }
        };

        template <typename T2> struct GenericDynamicCast {
            template <typename T1> T2* operator()(T1* p) const { return dynamic_cast<T2*>(p); }
            template <typename T1> std::shared_ptr<T2> operator()(const std::shared_ptr<T1>& p) const {
                return std::dynamic_pointer_cast<T2>(p);
            }
        };

        template <typename T2> struct GenericReinterpretCast {
            template <typename T1> T2* operator()(T1* p) const { return reinterpret_cast<T2*>(p); }
            template <typename T1> std::shared_ptr<T2> operator()(const std::shared_ptr<T1>& p) const {
                return std::reinterpret_pointer_cast<T2>(p);
            }
        };

        template <typename T2> struct GenericStaticCast {
            template <typename T1> T2* operator()(T1* p) const { return static_cast<T2*>(p); }
            template <typename T1> std::shared_ptr<T2> operator()(const std::shared_ptr<T1>& p) const {
                return std::static_pointer_cast<T2>(p);
            }
        };

    }

    template <typename T>
    struct ConstPtrObject:
    AlgorithmBase<ConstPtrObject<T>> {};

    template <typename T>
    struct DynamicPtrObject:
    AlgorithmBase<DynamicPtrObject<T>> {};

    template <typename T>
    struct ReinterpretPtrObject:
    AlgorithmBase<ReinterpretPtrObject<T>> {};

    template <typename T>
    struct StaticPtrObject:
    AlgorithmBase<StaticPtrObject<T>> {};

    template <typename Range, typename T>
    class ConstPtrIterator:
    public Detail::FlexibleIterator<ConstPtrIterator<Range, T>, const T> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = decltype(Detail::GenericConstCast<T>()(*std::declval<underlying_iterator>()));
        ConstPtrIterator() = default;
        ConstPtrIterator(underlying_iterator i): iter_(i), value_{} {}
        const value_type& operator*() const noexcept {
            if (! value_)
                value_ = Detail::GenericConstCast<T>()(*iter_);
            return value_;
        }
        ConstPtrIterator& operator++() { ++iter_; value_ = {}; return *this; }
        ConstPtrIterator& operator--() { --iter_; value_ = {}; return *this; }
        ConstPtrIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; value_ = {}; return *this; }
        ptrdiff_t operator-(const ConstPtrIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const ConstPtrIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        mutable value_type value_;
    };

    template <typename Range, typename T>
    class DynamicPtrIterator:
    public Detail::FlexibleIterator<DynamicPtrIterator<Range, T>, const T> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = decltype(Detail::GenericDynamicCast<T>()(*std::declval<underlying_iterator>()));
        DynamicPtrIterator() = default;
        DynamicPtrIterator(underlying_iterator i): iter_(i), value_{} {}
        const value_type& operator*() const noexcept {
            if (! value_)
                value_ = Detail::GenericDynamicCast<T>()(*iter_);
            return value_;
        }
        DynamicPtrIterator& operator++() { ++iter_; value_ = {}; return *this; }
        DynamicPtrIterator& operator--() { --iter_; value_ = {}; return *this; }
        DynamicPtrIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; value_ = {}; return *this; }
        ptrdiff_t operator-(const DynamicPtrIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const DynamicPtrIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        mutable value_type value_;
    };

    template <typename Range, typename T>
    class ReinterpretPtrIterator:
    public Detail::FlexibleIterator<ReinterpretPtrIterator<Range, T>, const T> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = decltype(Detail::GenericReinterpretCast<T>()(*std::declval<underlying_iterator>()));
        ReinterpretPtrIterator() = default;
        ReinterpretPtrIterator(underlying_iterator i): iter_(i), value_{} {}
        const value_type& operator*() const noexcept {
            if (! value_)
                value_ = Detail::GenericReinterpretCast<T>()(*iter_);
            return value_;
        }
        ReinterpretPtrIterator& operator++() { ++iter_; value_ = {}; return *this; }
        ReinterpretPtrIterator& operator--() { --iter_; value_ = {}; return *this; }
        ReinterpretPtrIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; value_ = {}; return *this; }
        ptrdiff_t operator-(const ReinterpretPtrIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const ReinterpretPtrIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        mutable value_type value_;
    };

    template <typename Range, typename T>
    class StaticPtrIterator:
    public Detail::FlexibleIterator<StaticPtrIterator<Range, T>, const T> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = decltype(Detail::GenericStaticCast<T>()(*std::declval<underlying_iterator>()));
        StaticPtrIterator() = default;
        StaticPtrIterator(underlying_iterator i): iter_(i), value_{} {}
        const value_type& operator*() const noexcept {
            if (! value_)
                value_ = Detail::GenericStaticCast<T>()(*iter_);
            return value_;
        }
        StaticPtrIterator& operator++() { ++iter_; value_ = {}; return *this; }
        StaticPtrIterator& operator--() { --iter_; value_ = {}; return *this; }
        StaticPtrIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; value_ = {}; return *this; }
        ptrdiff_t operator-(const StaticPtrIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const StaticPtrIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        mutable value_type value_;
    };

    template <typename Range, typename T>
    TL::Irange<ConstPtrIterator<Range, T>> operator>>(const Range& lhs, ConstPtrObject<T> /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    template <typename Range, typename T>
    TL::Irange<DynamicPtrIterator<Range, T>> operator>>(const Range& lhs, DynamicPtrObject<T> /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    template <typename Range, typename T>
    TL::Irange<ReinterpretPtrIterator<Range, T>> operator>>(const Range& lhs, ReinterpretPtrObject<T> /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    template <typename Range, typename T>
    TL::Irange<StaticPtrIterator<Range, T>> operator>>(const Range& lhs, StaticPtrObject<T> /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    template <typename ForwardRange, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, ConstPtrObject<T>) {
        auto r = Detail::as_range(lhs);
        std::transform(r.begin(), r.end(), r.begin(), Detail::GenericConstCast<T>());
        return lhs;
    }

    template <typename ForwardRange, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, DynamicPtrObject<T>) {
        auto r = Detail::as_range(lhs);
        std::transform(r.begin(), r.end(), r.begin(), Detail::GenericDynamicCast<T>());
        return lhs;
    }

    template <typename ForwardRange, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, ReinterpretPtrObject<T>) {
        auto r = Detail::as_range(lhs);
        std::transform(r.begin(), r.end(), r.begin(), Detail::GenericReinterpretCast<T>());
        return lhs;
    }

    template <typename ForwardRange, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, StaticPtrObject<T>) {
        auto r = Detail::as_range(lhs);
        std::transform(r.begin(), r.end(), r.begin(), Detail::GenericStaticCast<T>());
        return lhs;
    }

    template <typename T>
    inline ConstPtrObject<T> const_ptr = {};

    template <typename T>
    inline DynamicPtrObject<T> dynamic_ptr = {};

    template <typename T>
    inline ReinterpretPtrObject<T> reinterpret_ptr = {};

    template <typename T>
    inline StaticPtrObject<T> static_ptr = {};

    // construct, initialize

    template <typename T, bool Init>
    struct ConstructObject;

    template <typename T>
    struct ConstructObject<T, false>:
    AlgorithmBase<ConstructObject<T, false>> {
        template <typename... Args> T operator()(const Args&... args) const { return T(args...); }
    };

    template <typename T>
    struct ConstructObject<T, true>:
    AlgorithmBase<ConstructObject<T, true>> {
        template <typename... Args> T operator()(const Args&... args) const { return T{args...}; }
    };

    template <typename Range, typename T, bool Init>
    class ConstructIterator:
    public Detail::FlexibleIterator<ConstructIterator<Range, T, Init>, const T> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        ConstructIterator() = default;
        ConstructIterator(underlying_iterator i): iter_(i), value_(), ok_(false) {}
        const T& operator*() const noexcept {
            if (! ok_) {
                value_ = std::apply(ConstructObject<T, Init>(), *iter_);
                ok_ = true;
            }
            return value_;
        }
        ConstructIterator& operator++() { ++iter_; ok_ = false; return *this; }
        ConstructIterator& operator--() { --iter_; ok_ = false; return *this; }
        ConstructIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; ok_ = false; return *this; }
        ptrdiff_t operator-(const ConstructIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const ConstructIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        mutable T value_;
        mutable bool ok_;
    };

    template <typename Range, typename T, bool Init>
    TL::Irange<ConstructIterator<Range, T, Init>> operator>>(const Range& lhs, ConstructObject<T, Init> /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    template <typename T>
    inline ConstructObject<T, false> construct = {};

    template <typename T>
    inline ConstructObject<T, true> initialize = {};

    // convert

    template <typename T>
    struct ConvertObject:
    AlgorithmBase<ConvertObject<T>> {};

    template <typename Range, typename T>
    class ConvertIterator:
    public Detail::FlexibleIterator<ConvertIterator<Range, T>, const T> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        ConvertIterator() = default;
        ConvertIterator(underlying_iterator i): iter_(i), value_(), ok_(false) {}
        const T& operator*() const noexcept {
            if (! ok_) {
                value_ = static_cast<T>(*iter_);
                ok_ = true;
            }
            return value_;
        }
        ConvertIterator& operator++() { ++iter_; ok_ = false; return *this; }
        ConvertIterator& operator--() { --iter_; ok_ = false; return *this; }
        ConvertIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; ok_ = false; return *this; }
        ptrdiff_t operator-(const ConvertIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const ConvertIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        mutable T value_;
        mutable bool ok_;
    };

    template <typename Range, typename T>
    TL::Irange<ConvertIterator<Range, T>> operator>>(const Range& lhs, ConvertObject<T> /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    template <typename ForwardRange, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, ConvertObject<T>) {
        auto r = Detail::as_range(lhs);
        std::transform(r.begin(), r.end(), r.begin(), [] (auto& t) { return static_cast<T>(t); });
        return lhs;
    }

    template <typename T>
    inline ConvertObject<T> convert = {};

    // dereference

    struct DereferenceObject:
    AlgorithmBase<DereferenceObject> {};

    template <typename Range>
    class DereferenceIterator:
    public Detail::FlexibleIterator<DereferenceIterator<Range>, const std::decay_t<decltype(*std::declval<TL::RangeValue<Range>>())>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        DereferenceIterator() = default;
        DereferenceIterator(underlying_iterator i): iter_(i) {}
        const auto& operator*() const noexcept { return **iter_; }
        DereferenceIterator& operator++() { ++iter_; return *this; }
        DereferenceIterator& operator--() { --iter_; return *this; }
        DereferenceIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; return *this; }
        ptrdiff_t operator-(const DereferenceIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const DereferenceIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
    };

    template <typename Range>
    TL::Irange<DereferenceIterator<Range>> operator>>(const Range& lhs, DereferenceObject /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    constexpr DereferenceObject dereference = {};

    // indexed

    template <typename Integer>
    struct IndexedObject:
    AlgorithmBase<IndexedObject<Integer>> {
        Integer init = 0, delta = 1;
        template <typename I2> IndexedObject<I2> operator()(I2 i, I2 d = I2(1)) const {
            IndexedObject<I2> o;
            o.init = i;
            o.delta = d;
            return o;
        }
    };

    template <typename Range, typename Integer>
    class IndexedIterator:
    public TL::ForwardIterator<IndexedIterator<Range, Integer>, const std::pair<Integer, TL::RangeValue<Range>>> {
    public:
        using iterator_category = Detail::CommonCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = TL::RangeIterator<const Range>;
        using value_type = std::pair<Integer, TL::RangeValue<Range>>;
        IndexedIterator() = default;
        IndexedIterator(underlying_iterator i, Integer n, Integer d): iter_(i), key_(n), delta_(d), value_(), ok_(false) {}
        const value_type& operator*() const noexcept {
            if (! ok_) {
                value_ = std::make_pair(key_, *iter_);
                ok_ = true;
            }
            return value_;
        }
        IndexedIterator& operator++() { ++iter_; key_ += delta_; ok_ = false; return *this; }
        bool operator==(const IndexedIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        Integer key_;
        Integer delta_;
        mutable value_type value_;
        mutable bool ok_;
    };

    template <typename Range, typename Integer>
    TL::Irange<IndexedIterator<Range, Integer>> operator>>(const Range& lhs, IndexedObject<Integer> rhs) {
        auto r = Detail::as_range(lhs);
        return {{r.begin(), rhs.init, rhs.delta}, {r.end(), 0, 0}};
    }

    constexpr IndexedObject<int> indexed = {};

    // iterators

    struct IteratorsObject:
    AlgorithmBase<IteratorsObject> {};

    template <typename Range>
    class IteratorsIterator:
    public Detail::FlexibleIterator<IteratorsIterator<Range>, const TL::RangeIterator<Range>> {
    public:
        using underlying_iterator = TL::RangeIterator<Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        IteratorsIterator() = default;
        IteratorsIterator(underlying_iterator i): iter_(i) {}
        const auto& operator*() const noexcept { return iter_; }
        IteratorsIterator& operator++() { ++iter_; return *this; }
        IteratorsIterator& operator--() { --iter_; return *this; }
        IteratorsIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; return *this; }
        ptrdiff_t operator-(const IteratorsIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const IteratorsIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
    };

    template <typename Range>
    TL::Irange<IteratorsIterator<Range>> operator>>(Range& lhs, IteratorsObject /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    template <typename Range>
    TL::Irange<IteratorsIterator<const Range>> operator>>(const Range& lhs, IteratorsObject /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    constexpr IteratorsObject iterators = {};

    // keys, values

    template <bool Keys>
    struct KeysValuesObject:
    AlgorithmBase<KeysValuesObject<Keys>> {};

    template <typename Range, bool Keys>
    struct KeyValueType;

    template <typename Range>
    struct KeyValueType<Range, true> {
        using type = typename TL::RangeValue<Range>::first_type;
        static const type& get(const TL::RangeValue<Range>& rv) { return rv.first; }
    };

    template <typename Range>
    struct KeyValueType<Range, false> {
        using type = typename TL::RangeValue<Range>::second_type;
        static const type& get(const TL::RangeValue<Range>& rv) { return rv.second; }
    };

    template <typename Range, bool Keys>
    class KeyValueIterator:
    public Detail::FlexibleIterator<KeyValueIterator<Range, Keys>, const typename KeyValueType<Range, Keys>::type> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        KeyValueIterator() = default;
        KeyValueIterator(underlying_iterator i): iter_(i) {}
        const auto& operator*() const noexcept { return KeyValueType<Range, Keys>::get(*iter_); }
        KeyValueIterator& operator++() { ++iter_; return *this; }
        KeyValueIterator& operator--() { --iter_; return *this; }
        KeyValueIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; return *this; }
        ptrdiff_t operator-(const KeyValueIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const KeyValueIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
    };

    template <typename Range, bool Keys>
    TL::Irange<KeyValueIterator<Range, Keys>> operator>>(const Range& lhs, KeysValuesObject<Keys> /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    constexpr KeysValuesObject<true> keys = {};
    constexpr KeysValuesObject<false> values = {};

    // map, map_pairs

    template <typename UnaryFunction>
    struct MapObject:
    AlgorithmBase<MapObject<UnaryFunction>> {
        UnaryFunction fun;
        MapObject(const UnaryFunction& f): fun(f) {}
    };

    template <typename BinaryFunction>
    struct MapPairsObject:
    AlgorithmBase<MapPairsObject<BinaryFunction>> {
        BinaryFunction fun;
        MapPairsObject(const BinaryFunction& f): fun(f) {}
    };

    template <typename Range, typename UnaryFunction>
    class MapIterator:
    public Detail::FlexibleIterator<MapIterator<Range, UnaryFunction>,
        const std::invoke_result_t<UnaryFunction, TL::RangeValue<Range>>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = std::invoke_result_t<UnaryFunction, TL::RangeValue<Range>>;
        using function_type = std::function<value_type(const TL::RangeValue<Range>&)>;
        MapIterator() = default;
        MapIterator(underlying_iterator i, UnaryFunction f): iter_(i), fun_(f), value_(), ok_(false) {}
        const value_type& operator*() const { if (! ok_) { value_ = fun_(*iter_); ok_ = true; } return value_; }
        MapIterator& operator++() { ++iter_; ok_ = false; return *this; }
        MapIterator& operator--() { --iter_; ok_ = false; return *this; }
        MapIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; ok_ = false; return *this; }
        ptrdiff_t operator-(const MapIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const MapIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        function_type fun_;
        mutable value_type value_;
        mutable bool ok_;
    };

    template <typename Range, typename BinaryFunction>
    class MapPairsIterator:
    public Detail::FlexibleIterator<MapPairsIterator<Range, BinaryFunction>,
        const std::invoke_result_t<BinaryFunction, typename TL::RangeValue<Range>::first_type,
        typename TL::RangeValue<Range>::second_type>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using first_type = typename TL::RangeValue<Range>::first_type;
        using second_type = typename TL::RangeValue<Range>::second_type;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = std::invoke_result_t<BinaryFunction, first_type, second_type>;
        using function_type = std::function<value_type(const first_type&, const second_type&)>;
        MapPairsIterator() = default;
        MapPairsIterator(underlying_iterator i, BinaryFunction f): iter_(i), fun_(f), value_(), ok_(false) {}
        const value_type& operator*() const { if (! ok_) { value_ = fun_(iter_->first, iter_->second); ok_ = true; } return value_; }
        MapPairsIterator& operator++() { ++iter_; ok_ = false; return *this; }
        MapPairsIterator& operator--() { --iter_; ok_ = false; return *this; }
        MapPairsIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; ok_ = false; return *this; }
        ptrdiff_t operator-(const MapPairsIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const MapPairsIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        function_type fun_;
        mutable value_type value_;
        mutable bool ok_;
    };

    template <typename Range, typename UnaryFunction>
    TL::Irange<MapIterator<Range, UnaryFunction>> operator>>(const Range& lhs, MapObject<UnaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        return {{r.begin(), rhs.fun}, {r.end(), rhs.fun}};
    }

    template <typename Range, typename BinaryFunction>
    TL::Irange<MapPairsIterator<Range, BinaryFunction>> operator>>(const Range& lhs, MapPairsObject<BinaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        return {{r.begin(), rhs.fun}, {r.end(), rhs.fun}};
    }

    template <typename ForwardRange, typename UnaryFunction>
    ForwardRange& operator<<(ForwardRange& lhs, MapObject<UnaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        std::transform(r.begin(), r.end(), r.begin(), rhs.fun);
        return lhs;
    }

    template <typename UnaryFunction>
    inline MapObject<UnaryFunction> map(UnaryFunction f) {
        return f;
    }

    template <typename BinaryFunction>
    inline MapPairsObject<BinaryFunction> map_pairs(BinaryFunction f) {
        return f;
    }

    // map_if

    template <typename Predicate, typename UnaryFunction>
    struct MapIfObject:
    AlgorithmBase<MapIfObject<Predicate, UnaryFunction>> {
        Predicate pred;
        UnaryFunction fun;
        MapIfObject(const Predicate& p, const UnaryFunction& f): pred(p), fun(f) {}
    };

    template <typename Range, typename Predicate, typename UnaryFunction>
    class MapIfIterator:
    public Detail::FlexibleIterator<MapIfIterator<Range, Predicate, UnaryFunction>, TL::RangeValue<Range>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = TL::RangeValue<Range>;
        using predicate_type = std::function<bool(const value_type&)>;
        using function_type = std::function<value_type(const value_type&)>;
        MapIfIterator() = default;
        MapIfIterator(underlying_iterator i, Predicate p, UnaryFunction f): iter_(i), pred_(p), fun_(f), value_(), ok_(false) {}
        const value_type& operator*() const {
            if (! ok_) {
                if (pred_(*iter_))
                    value_ = fun_(*iter_);
                else
                    value_ = *iter_;
                ok_ = true;
            }
            return value_;
        }
        MapIfIterator& operator++() { ++iter_; ok_ = false; return *this; }
        MapIfIterator& operator--() { --iter_; ok_ = false; return *this; }
        MapIfIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; ok_ = false; return *this; }
        ptrdiff_t operator-(const MapIfIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const MapIfIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        predicate_type pred_;
        function_type fun_;
        mutable value_type value_;
        mutable bool ok_;
    };

    template <typename Range, typename Predicate, typename UnaryFunction>
    TL::Irange<MapIfIterator<Range, Predicate, UnaryFunction>> operator>>(const Range& lhs, MapIfObject<Predicate, UnaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        return {{r.begin(), rhs.pred, rhs.fun}, {r.end(), rhs.pred, rhs.fun}};
    }

    template <typename ForwardRange, typename Predicate, typename UnaryFunction>
    ForwardRange& operator<<(ForwardRange& lhs, MapIfObject<Predicate, UnaryFunction> rhs) {
        auto r = Detail::as_range(lhs);
        std::for_each(r.begin(), r.end(), [&] (auto& x) { if (rhs.pred(x)) x = rhs.fun(x); });
        return lhs;
    }

    template <typename Predicate, typename UnaryFunction>
    inline MapIfObject<Predicate, UnaryFunction> map_if(Predicate p, UnaryFunction f) {
        return {p, f};
    }

    // replace, replace_if

    template <typename Predicate, typename T>
    struct ReplaceObject:
    AlgorithmBase<ReplaceObject<Predicate, T>> {
        Predicate pred;
        T subst;
        ReplaceObject(const Predicate& p, const T& t): pred(p), subst(t) {}
    };

    template <typename Range, typename Predicate>
    class ReplaceIterator:
    public Detail::FlexibleIterator<ReplaceIterator<Range, Predicate>, const TL::RangeValue<Range>> {
    public:
        using underlying_iterator = TL::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = TL::RangeValue<Range>;
        using predicate_type = std::function<bool(const value_type&)>;
        ReplaceIterator() = default;
        template <typename T> ReplaceIterator(underlying_iterator i, Predicate p, const T& t): iter_(i), pred_(p), subst_(t) {}
        const value_type& operator*() const { if (pred_(*iter_)) return subst_; else return *iter_; }
        ReplaceIterator& operator++() { ++iter_; return *this; }
        ReplaceIterator& operator--() { --iter_; return *this; }
        ReplaceIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; return *this; }
        ptrdiff_t operator-(const ReplaceIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const ReplaceIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        predicate_type pred_;
        value_type subst_;
    };

    template <typename Range, typename Predicate, typename T>
    TL::Irange<ReplaceIterator<Range, Predicate>> operator>>(const Range& lhs, const ReplaceObject<Predicate, T>& rhs) {
        auto r = Detail::as_range(lhs);
        return {{r.begin(), rhs.pred, rhs.subst}, {r.end(), rhs.pred, rhs.subst}};
    }

    template <typename ForwardRange, typename Predicate, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, ReplaceObject<Predicate, T> rhs) {
        auto r = Detail::as_range(lhs);
        TL::RangeValue<ForwardRange> s = rhs.subst;
        std::replace_if(r.begin(), r.end(), rhs.pred, s);
        return lhs;
    }

    template <typename Predicate, typename T>
    inline ReplaceObject<Predicate, T> replace_if(Predicate p, const T& t2) {
        return {p, t2};
    }

    template <typename T1, typename T2>
    inline auto replace(const T1& t1, const T2& t2) {
        return replace_if([t1] (auto& x ) { return x == t1; }, t2);
    }

    // swap_pairs

    struct SwapPairsObject:
    AlgorithmBase<SwapPairsObject> {};

    template <typename PairRange>
    struct SwapPairsType {
        using type1 = std::decay_t<typename TL::RangeValue<PairRange>::first_type>;
        using type2 = std::decay_t<typename TL::RangeValue<PairRange>::second_type>;
        using value_type = std::pair<type2, type1>;
    };

    template <typename PairRange>
    class SwapPairsIterator:
    public Detail::FlexibleIterator<SwapPairsIterator<PairRange>, const typename SwapPairsType<PairRange>::value_type> {
    public:
        using underlying_iterator = TL::RangeIterator<const PairRange>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = typename SwapPairsType<PairRange>::value_type;
        SwapPairsIterator() = default;
        SwapPairsIterator(underlying_iterator i): iter_(i), value_(), ok_(false) {}
        const value_type& operator*() const noexcept {
            if (! ok_) {
                value_ = std::make_pair(iter_->second, iter_->first);
                ok_ = true;
            }
            return value_;
        }
        SwapPairsIterator& operator++() { ++iter_; ok_ = false; return *this; }
        SwapPairsIterator& operator--() { --iter_; ok_ = false; return *this; }
        SwapPairsIterator& operator+=(ptrdiff_t rhs) { iter_ += rhs; ok_ = false; return *this; }
        ptrdiff_t operator-(const SwapPairsIterator& rhs) const { return iter_ - rhs.iter_; }
        bool operator==(const SwapPairsIterator& rhs) const noexcept { return iter_ == rhs.iter_; }
    private:
        underlying_iterator iter_;
        mutable value_type value_;
        mutable bool ok_;
    };

    template <typename PairRange>
    TL::Irange<SwapPairsIterator<PairRange>> operator>>(const PairRange& lhs, SwapPairsObject /*rhs*/) {
        auto r = Detail::as_range(lhs);
        return {{r.begin()}, {r.end()}};
    }

    template <typename PairContainer>
    PairContainer& operator<<(PairContainer& lhs, SwapPairsObject /*rhs*/) {
        PairContainer temp;
        lhs >> SwapPairsObject() >> append(temp);
        lhs = std::move(temp);
        return lhs;
    }

    constexpr SwapPairsObject swap_pairs = {};

}
