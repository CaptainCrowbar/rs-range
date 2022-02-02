# Range Algorithms Library

By Ross Smith

_[GitHub repository](https://github.com/CaptainCrowbar/rs-range)_

## Overview

```c++
#include "rs-range.hpp"
namespace RS::Range;
```

This is my range algorithms library.

The CMake file includes an `install` target to copy the headers into
`/usr/local/{bin,include}` or the equivalent. Headers can be included
individually as required, or the entire library can be included using
`"rs-range.hpp"`.

Dependencies:

* [My format library](https://github.com/CaptainCrowbar/rs-format)
* [My template library](https://github.com/CaptainCrowbar/rs-tl)
* [My unit test library](https://github.com/CaptainCrowbar/rs-unit-test)

## Overview

### Pipeline algorithms

Pipeline algorithms take the form:

```c++
result = range >> algorithm;
```

For example, this code will sort a vector of strings into alphabetical order,
remove duplicates, and copy the first 10 into the output vector:

```c++
vector<string> v1, v2;
v1 >> sort >> unique >> take(10) >> append(v2);
```

Most algorithms take a range as input and return another range. Some also take
additional parameters, passed as function arguments to the algorithm. If an
algorithm takes no arguments, its name can be used alone with no parentheses.

Usually the output range is a virtual range composed of iterators that either
point back into the original input range or generate their values on the fly.
When the output is described as a temporary range, the actual return value is
a pair of iterators that carry a shared, reference counted handle to a new
container constructed by the algorithm. The container will exist as long as
the iterators, or copies of them, exist.

The algorithm descriptions below indicate the minimum range category (input,
forward, bidirectional, or random access) required of the input, and the
category, or range of categories, of the output range. If the output is just
described as a range with no information about category, then the output range
has the same category as the input. An output range with a conditional
category has that category only if the input range has at least the same
category; otherwise, the output range has the same category as the input
range. For example, a conditional bidirectional range (CBR) will be
bidirectional if the input range is bidirectional or random access, but will
only be input or forward if the input range is input or forward.

### Reflex algorithms

Reflex algorithms take the form:

```c++
range& << algorithm;
```

A reflex algorithm is one that writes its output back into its input range or
container, instead of returning a new range. Using an algorithm in reflex form
requires that its output elements have the same type as its input elements, or
an assignment compatible type. The collection operated on by a reflex
algorithm can be a writeable range if there is a one to one mapping from input
to output elements; otherwise, it must be an STL compatible container. The
return value from a reflex algorithm is always a reference to the updated
input range.

The same example as above:

```c++
vector<string> v;
v << sort << unique << take(10);
```

### Composite algorithms

Composite algorithms take the form

```c++
composite = algorithm1 * algorithm2 * ...;
```

Algorithms can be composed using the binary `*` operator; the algorithms in a
composite are applied from left to right. Composite algorithms can be used in
pipeline mode, or in reflex mode if all of their component algorithms have
reflex forms.

Example:

```c++
vector<string> v1, v2;
auto top10 = sort * unique * take(10);
v1 >> top10 >> append(v2);
```

### Categorization

* **Core algorithms** are fundamental operations that have special behaviour or otherwise don't fit into any of the other categories.
* **Aggregation algorithms** combine multiple input elements into each output element.
* **Combination algorithms** generate a single output range from multiple input ranges.
* **Expansion algorithms** generate multiple output elements from each input element.
* **Generation algorithms** create an output range without taking an input range.
* **Permutation algorithms** return a permutation of the input range.
* **Reduction algorithms** take an input range and return a scalar result.
* **Selection algorithms** return a subset of the input range.
* **Transformation algorithms** apply a one-to-one mapping from the input range to the output range.

## Algorithms reference table

Columns in the table:

* Category -- These are the algorithm categories listed above.
* Algorithm -- The algorithm name.
* Args -- The arguments taken by the algorithm, excluding the input and output ranges.
* Pipeline -- The input and output range types, when the algorithm is used in pipeline mode.
    * e.g. `FR -> R` indicates that the algorithm takes a range of at least forward category as input,
        and returns a range of the same category.
* Reflex -- The input range type, when the algorithm is used in reflex mode.
    The output in reflex mode is always a reference to the modified input.

Abbreviations used:

* Range categories:
    * `R` = Range (unrestricted)
    * `IR` = Input (single pass) range
    * `FR` = Forward range
    * `BR` = Bidirectional range
    * `RR` = Random access range
* Iterator categories:
    * `I` = Iterator (unrestricted)
    * `II` = Input iterator
    * `OI` = Output iterator
    * `FI` = Forward iterator
    * `BI` = Bidirectional iterator
    * `RI` = Random access iterator
* Range modifiers:
    * `C_R` = Conditional range
    * `N_R` = Nested range (each element is itself a range)
    * `S_R` = Sorted range (assumed to be sorted on a comparison predicate)
    * `P_R` = Pair range (each element is a `std::pair`, or other type with `first` and `second` members)
    * `T_R` = Temporary range
* Function types:
    * `GF` = Generator function: `f()->x`
    * `UF` = Unary function: `f(x)->y`
    * `BF` = Binary function: `f(x,y)->z`
    * `UP` = Unary predicate: `f(x)->bool`
    * `BP` = Binary predicate: `f(x,y)->bool`
    * `EP` = Equality predicate (binary predicate with equality semantics)
    * `CP` = Comparison predicate (binary predicate with less-than semantics)
    * `RD` = Random distribution: `f(rng&)->x`
    * `RNG` = Random number engine: `f()->n`
* Other types:
    * `C` = Container type (STL compatible)
    * `FP` = Floating point type
    * `N` = Integer type
    * `T` = Value type of a range

| Category        | Algorithm                   | Args                 | Pipeline       | Reflex  |
| --------        | ---------                   | ----                 | --------       | ------  |
| Core            | `append`                    | `(C&)`               | `R -> R`       | _none_  |
|                 | `collect`                   | _none_               | `R -> TRR`     | _none_  |
|                 | `collect_as`                | `<C>`                | `R -> C`       | _none_  |
|                 | `each`                      | `(UF)`               | `R -> R`       | `R&`    |
|                 | `each_pair`                 | `(BF)`               | `PR -> R`      | `PR&`   |
|                 | `output`                    | `(OI)`               | `R -> OI`      | _none_  |
|                 | `overwrite`                 | `(C&)`               | `R -> R`       | _none_  |
|                 | `passthrough`               | _none_               | `R -> R`       | `R&`    |
| Aggregation     | `adjacent_difference`       | `[(BF)]`             | `R -> CFR`     | `C&`    |
|                 | `census`                    | `[(EP)]`             | `R -> CPFR`    | _none_  |
|                 | `collect_groups`            | `(BP[,BF])`          | `FR -> FR`     | `C&`    |
|                 | `collect_groups_by`         | `(UF[,BF])`          | `FR -> FR`     | `C&`    |
|                 | `group`                     | `[(EP)]`             | `FR -> NFR`    | _none_  |
|                 | `group_by`                  | `(UF)`               | `FR -> NFR`    | _none_  |
|                 | `group_k`                   | `(N)`                | `FR -> NFR`    | _none_  |
|                 | `partial_sum`               | `[(BF)]`             | `R -> CFR`     | `C&`    |
| Combination     | `compare`                   | `(R[,CP])`           | `R -> bool`    | _none_  |
|                 | `compare_3way`              | `(R[,CP])`           | `R -> int`     | _none_  |
|                 | `concat`                    | `(R)`                | `R -> CFR`     | `C&`    |
|                 | `inner_product`             | `(R[,T,BF,BF])`      | `R -> T`       | _none_  |
|                 | `interleave`                | `(R)`                | `R -> CFR`     | _none_  |
|                 | `is_equal`                  | `(R[,EP])`           | `R -> bool`    | _none_  |
|                 | `is_equivalent`             | `(R[,CP])`           | `R -> bool`    | _none_  |
|                 | `merge`                     | `(R[,CP])`           | `SR -> CSFR`   | `SC&`   |
|                 | `outer_product`             | `(FR[,BF])`          | `FR -> FR`     | _none_  |
|                 | `prefix`                    | `(R)`                | `R -> CFR`     | `C&`    |
|                 | `self_cross`                | `[(BF)]`             | `FR -> FR`     | _none_  |
|                 | `set_difference`            | `(OR[,CP])`          | `SR -> CSFR`   | `SC&`   |
|                 | `set_difference_from`       | `(OR[,CP])`          | `SR -> CSFR`   | `SC&`   |
|                 | `set_intersection`          | `(OR[,CP])`          | `SR -> CSFR`   | `SC&`   |
|                 | `set_symmetric_difference`  | `(OR[,CP])`          | `SR -> CSFR`   | `SC&`   |
|                 | `set_union`                 | `(OR[,CP])`          | `SR -> CSFR`   | `SC&`   |
|                 | `zip`                       | `(R[,BF])`           | `R -> CFR`     | _none_  |
| Expansion       | `combinations`              | `(N)`                | `FR -> NIR`    | _none_  |
|                 | `flat_map`                  | `(UF)`               | `R -> CFR`     | `FR&`   |
|                 | `flatten`                   | _none_               | `NR -> CFR`    | _none_  |
|                 | `insert_after`              | `(T)`                | `R -> CFR`     | `FR&`   |
|                 | `insert_around`             | `(T,T)`              | `R -> CFR`     | `FR&`   |
|                 | `insert_before`             | `(T)`                | `R -> CFR`     | `FR&`   |
|                 | `insert_between`            | `(T)`                | `R -> CFR`     | `FR&`   |
|                 | `permutations`              | `[(CP)]`             | `R -> NIR`     | _none_  |
|                 | `repeat`                    | `[(N)]`              | `FR -> CBR`    | `C&`    |
|                 | `subsets`                   | `(N)`                | `FR -> NIR`    | _none_  |
| Generation      | `epsilon<T>`                | _none_               | `void -> RR`   | _none_  |
|                 | `fill`                      | `(T[,N])`            | `void -> RR`   | `R&`    |
|                 | `generate`                  | `(GF[,N])`           | `void -> IR`   | `R&`    |
|                 | `iota`                      | `(T[,T_or_UF[,N]])`  | `void -> FR`   | `R&`    |
|                 | `random`                    | `(RD,RNG[,N])`       | `void -> IR`   | `R&`    |
|                 | `single`                    | `(T)`                | `void -> RR`   | _none_  |
| Permutation     | `next_permutation`          | `[(CP)]`             | `R -> TRR`     | `BR&`   |
|                 | `prev_permutation`          | `[(CP)]`             | `R -> TRR`     | `BR&`   |
|                 | `reverse`                   | _none_               | `BR -> BR`     | `BR&`   |
|                 | `shuffle`                   | `(RNG)`              | `R -> TRR`     | `RR&`   |
|                 | `sort`                      | `[(CP)]`             | `R -> TRR`     | `RR&`   |
|                 | `stable_sort`               | `[(CP)]`             | `R -> TRR`     | `RR&`   |
| Reduction       | `all_of`                    | `(UP)`               | `R -> bool`    | _none_  |
|                 | `any_of`                    | `(UP)`               | `R -> bool`    | _none_  |
|                 | `count`                     | `[(T)]`              | `R -> size_t`  | _none_  |
|                 | `count_if`                  | `(UP)`               | `R -> size_t`  | _none_  |
|                 | `fold_left`                 | `(T,BF)`             | `R -> T`       | _none_  |
|                 | `fold_right`                | `(T,BF)`             | `BR -> T`      | _none_  |
|                 | `is_empty`                  | _none_               | `R -> bool`    | _none_  |
|                 | `is_nonempty`               | _none_               | `R -> bool`    | _none_  |
|                 | `is_sorted`                 | `[(CP)]`             | `R -> bool`    | _none_  |
|                 | `max`                       | `[(CP)]`             | `R -> T`       | _none_  |
|                 | `min`                       | `[(CP)]`             | `R -> T`       | _none_  |
|                 | `min_max`                   | `[(CP)]`             | `R -> pair`    | _none_  |
|                 | `none_of`                   | `(UP)`               | `R -> bool`    | _none_  |
|                 | `product`                   | `[(T)]`              | `R -> T`       | _none_  |
|                 | `reduce`                    | `(BF)`               | `R -> T`       | _none_  |
|                 | `sum`                       | `[(T)]`              | `R -> T`       | _none_  |
| Selection       | `after`                     | `(T)`                | `R -> CFR`     | `C&`    |
|                 | `after_if`                  | `(UP)`               | `R -> CFR`     | `C&`    |
|                 | `before`                    | `(T)`                | `R -> CFR`     | `C&`    |
|                 | `before_if`                 | `(UP)`               | `R -> CFR`     | `C&`    |
|                 | `filter`                    | `(UP)`               | `R -> CFR`     | `C&`    |
|                 | `filter_out`                | `(UP)`               | `R -> CFR`     | `C&`    |
|                 | `from`                      | `(T)`                | `R -> CFR`     | `C&`    |
|                 | `from_if`                   | `(UP)`               | `R -> CFR`     | `C&`    |
|                 | `not_null`                  | _none_               | `R -> CFR`     | `C&`    |
|                 | `remove`                    | `(T)`                | `R -> CFR`     | `C&`    |
|                 | `sample_k`                  | `(N,RNG)`            | `RR -> TRR`    | `C&`    |
|                 | `sample_p`                  | `(FP,RNG)`           | `R -> CFR`     | `C&`    |
|                 | `sample_replace`            | `(N,RNG)`            | `RR -> TRR`    | `C&`    |
|                 | `skip`                      | `(N)`                | `R -> CFR`     | `C&`    |
|                 | `stride`                    | `(N[,N])`            | `R -> CFR`     | `C&`    |
|                 | `take`                      | `(N)`                | `R -> CFR`     | `C&`    |
|                 | `unique`                    | `[(EP)]`             | `R -> CFR`     | `C&`    |
|                 | `until`                     | `(T)`                | `R -> CFR`     | `C&`    |
|                 | `until_if`                  | `(UP)`               | `R -> CFR`     | `C&`    |
| Transformation  | `const_ptr`                 | `<T>`                | `R -> R`       | `FR&`   |
|                 | `construct`                 | `<T>`                | `R -> R`       | _none_  |
|                 | `convert`                   | `<T>`                | `R -> R`       | `FR&`   |
|                 | `dereference`               | _none_               | `R -> R`       | _none_  |
|                 | `dynamic_ptr`               | `<T>`                | `R -> R`       | `FR&`   |
|                 | `indexed`                   | `[(N[,N])]`          | `R -> CFR`     | _none_  |
|                 | `initialize`                | `<T>`                | `R -> R`       | _none_  |
|                 | `iterators`                 | _none_               | `R -> R`       | _none_  |
|                 | `keys`                      | _none_               | `R -> R`       | _none_  |
|                 | `map`                       | `(UF)`               | `R -> R`       | `FR&`   |
|                 | `map_if`                    | `(UP,UF)`            | `R -> R`       | `FR&`   |
|                 | `map_pairs`                 | `(BF)`               | `PR -> R`      | _none_  |
|                 | `reinterpret_ptr`           | `<T>`                | `R -> R`       | `FR&`   |
|                 | `replace`                   | `(T,T)`              | `R -> R`       | `FR&`   |
|                 | `replace_if`                | `(UP,T)`             | `R -> R`       | `FR&`   |
|                 | `static_ptr`                | `<T>`                | `R -> R`       | `FR&`   |
|                 | `swap_pairs`                | _none_               | `PR -> PR`     | `PC&`   |
|                 | `values`                    | _none_               | `R -> R`       | _none_  |

## Algorithm details

In the descriptions, `n` is the length of the input range (which may be
infinite in some cases); `m` is the length of the second input range (for
combination algorithms); `x` and `y` are input element values.

### Core algorithms

```c++
Range r >> append(Container& c) -> Range2;
Range r >> overwrite(Container& c) -> Range2;
```

Copy a range onto the end of a container; `overwrite()` clears the container
first.

```c++
Range r >> collect -> TemporaryRandomAccessRange;
```

Copy a virtual range into a concrete one. This is mainly useful for turning a
lower category into a random access range needed by the next stage in the
chain. Behaviour is undefined if the input range is infinite.

```c++
Range r >> collect_as<Container>() -> Container;
```

Copy a virtual range into a concrete one of a specific type.

```c++
Range r >> each(UnaryFunction f) -> Range2;
PairRange r >> each_pair(BinaryFunction f) -> Range2;
Range& r << each(UnaryFunction f);
PairRange& r << each_pair(BinaryFunction f);
```

Call a function on each element of a range, returning a reference to the input
range. The `each_pair()` algorithm calls `f(x.first,x.second)` for each
element.

```c++
Range r >> output(OutputIterator i);
```

Copy a range into an output iterator. Behaviour is undefined if the input
range is infinite.

```c++
Range r >> passthrough -> Range;
Range& r << passthrough;
```

An empty do-nothing algorithm that just passes on its input range.

### Aggregation algorithms

_Aggregation algorithms combine multiple input elements into each output element_

```c++
Range r >> adjacent_difference(BinaryFunction f = std::minus)
    -> ConditionalForwardRange;
Container& c << adjacent_difference(BinaryFunction f = std::minus);
```

Return `f(x,y)` for each pair of adjacent input elements. The output range
will be empty if the input contains less than two elements.

```c++
Range r >> census(EqualityPredicate p = std::equal_to)
    -> ConditinalForwardPairRange;
```

Return a `(value,count)` pair for each subsequence of adjacent equal values in
the input range.

```c++
ForwardRange r >> collect_groups(BinaryPredicate bp,
    BinaryFunction bf = std::plus) -> ForwardRange;
ForwardRange r >> collect_groups_by(UnaryFunction uf,
    BinaryFunction bf = std::plus) -> ForwardRange;
Container& c << collect_groups(BinaryPredicate bp,
    BinaryFunction bf = std::plus);
Container& c << collect_groups_by(UnaryFunction uf,
    BinaryFunction bf = std::plus);
```

Replace each group of elements in the range with an aggregate value (the sum
by default). The `collect_groups()` function matches values using the binary
predicate, while `collect_groups_by()` matches values for with the unary
function gives the same result. When two elements are combined, the combined
value (not the second of the two original values) is used as the LHS when
matching against the following element.

```c++
ForwardRange r >> group(EqualityPredicate p = std::equal_to)
    -> ForwardNestedRange;
ForwardRange r >> group_by(UnaryFunction f) -> ForwardNestedRange;
ForwardRange r >> group_k(size_t k) -> ForwardNestedRange;
```

Return a subrange (in the form of an `Irange` object containing an iterator
pair) for each group of elements in the input range. The `group()` algorithm
groups elements that are equal according to an equality predicate;
`group_by()` groups elements that return the same value for the given
function; `group_k()` groups elements in blocks of `k` without regard to their
value (the last group will be smaller if the length of the input range is not
a multiple of `k`).

```c++
Range r >> partial_sum(BinaryFunction f = std::plus)
    -> ConditionalForwardRange;
Container& c << partial_sum(BinaryFunction f = std::plus);
```

Return a sequence of partial sums of the input range. The output range is the
same length as the input.

### Combination algorithms

_Combination algorithms generate their output range from multiple input ranges_

```c++
Range r >> compare(Range2 r2, ComparisonPredicate p = std::less)
    -> bool;
Range r >> compare_3way(Range2 r2, ComparisonPredicate p = std::less)
    -> int;
Range r >> is_equal(Range2 r2, EqualityPredicate p = std::equal_to)
    -> bool;
Range r >> is_equivalent(Range2 r2, ComparisonPredicate p = std::less)
    -> bool;
```

Lexicographical comparison of two ranges. The `is_equal()`, `compare()`, and
`compare_3way()` algorithms are equivalent to `std::equal()`,
`std::lexicographical_compare()`, and `std::lexicographical_compare_3way()`
respectively; `is_equivalent()` performs an equality comparison using the
equivalence relation defined by the comparison predicate.

```c++
Range r >> concat(Range2 r2) -> ConditionalForwardRange;
Range r >> prefix(Range2 r2) -> ConditionalForwardRange;
Container& c << concat(Range2 r2);
Container& c << prefix(Range2 r2);
```

Concatenate two ranges; `concat()` returns `r` followed by `r2`, while
`prefix()` returns `r2` followed by `r`.

```c++
Range r >> inner_product(Range2 r2, T init = T(),
    BinaryFunction1 f1 = std::multiplies,
    BinaryFunction2 f2 = std::plus) -> T;
```

Calculates the inner product of two ranges, following the same conventions as
`std::inner_product()`. If the two ranges have different lengths, the extra
elements in the longer range are ignored.

```c++
Range r >> interleave(Range2 r2) -> ConditionalForwardRange;
```

Returns a range containing alternating elements of `r` and `r2`, starting with
the first element of `r`. If the two ranges have different lengths, the extra
elements in the longer range will be returned consecutively once the shorter
range is exhausted.

```c++
SortedRange r >> merge(SortedRange2 r2,
    ComparisonPredicate p = std::less) -> ConditionalForwardSortedRange;
SortedContainer& c << merge(SortedRange2 r2,
    ComparisonPredicate p = std::less);
```

Merge two ordered ranges.

```c++
ForwardRange r >> outer_product(ForwardRange2 r2,
    BinaryFunction f = std::make_pair) -> ForwardRange3;
ForwardRange r >> self_cross(BinaryFunction f = std::make_pair)
    -> ForwardRange2;
```

Return the outer product of two ranges, as a range containing `f(x,y)` for
every possible pair of elements from the two ranges, with the second argument
varying faster than the first; the output has `n*m` elements. The
`self_cross()` algorithm returns the cross product of the input range with
itself.

```c++
SortedRange r >> set_difference(SortedRange2 r2,
    ComparisonPredicate p = std::less) -> ConditionalForwardSortedRange;
SortedRange r >> set_difference_from(SortedRange2 r2,
    ComparisonPredicate p = std::less) -> ConditionalForwardSortedRange;
SortedRange r >> set_intersection(SortedRange2 r2,
    ComparisonPredicate p = std::less) -> ConditionalForwardSortedRange;
SortedRange r >> set_symmetric_difference(SortedRange2 r2,
    ComparisonPredicate p = std::less) -> ConditionalForwardSortedRange;
SortedRange r >> set_union(SortedRange2 r2,
    ComparisonPredicate p = std::less) -> ConditionalForwardSortedRange;
SortedContainer& c << set_difference(SortedRange2 r2,
    ComparisonPredicate p = std::less);
SortedContainer& c << set_difference_from(SortedRange2 r2,
    ComparisonPredicate p = std::less);
SortedContainer& c << set_intersection(SortedRange2 r2,
    ComparisonPredicate p = std::less);
SortedContainer& c << set_symmetric_difference(SortedRange2 r2,
    ComparisonPredicate p = std::less);
SortedContainer& c << set_union(SortedRange2 r2,
    ComparisonPredicate p = std::less);
```

Set operations on two ordered ranges. These perform the same operations as the
similarly named standard algorithms; `set_difference_from()` returns `r2-r`,
while `set_difference()` returns `r-r2`.

```c++
Range r >> zip(Range2 r2, BinaryFunction f = std::make_pair)
    -> ConditionalForwardRange;
```

Returns a range consisting of `f(x,y)` for each pair of elements, taking one
argument from each range. If the two ranges have different lengths, the extra
elements in the longer range are ignored.

### Expansion algorithms

_Expansion algorithms return multiple output elements from each input element_

```c++
ForwardRange r >> combinations(size_t k) -> InputNestedRange;
```

The output consists of all possible sequences of `k` elements (not necessarily
distinct) from the input range. The output contains <code>n<sup>k</sup></code>
elements, each a range of length `k`.

```c++
Range r >> flat_map(UnaryFunction f) -> ConditionalForwardRange;
ForwardRange& r << flat_map(UnaryFunction f);
```

Calls `f(x)` on each element, expecting the function to return some form of
container or range; the individual elements are then returned in the output
range.

```c++
NestedRange r >> flatten -> ConditionalForwardRange;
```

The elements of the input range are expected to be ranges themselves; the
output range is assembled by concatenating them.

```c++
Range r >> insert_after(T t) -> ConditionalForwardRange;
Range r >> insert_around(T t1, T t2) -> ConditionalForwardRange;
Range r >> insert_before(T t) -> ConditionalForwardRange;
Range r >> insert_between(T t) -> ConditionalForwardRange;
ForwardRange& r << insert_after(T t);
ForwardRange& r << insert_around(T t1, T t2);
ForwardRange& r << insert_before(T t);
ForwardRange& r << insert_between(T t);
```

Insert new elements into a range. The `insert_before()` and `insert_after()`
functions add `t` before or after each input element, yielding a range of
length `2n`; `insert_between()` adds `t` between adjacent elements, yielding a
range of length `2n-1` (or `n` if `n<2`); `insert_around()` adds `t1` before
and `t2` after each element, yielding a range of length `3n`.

```c++
Range r >> permutations(ComparisonPredicate p = std::less)
    -> InputNestedRange;
```

Generates all permutations of the input range, in lexicographical order,
starting with a copy of the input; the output contains `n!` elements, each a
collection of the same size as the input.

```c++
ForwardRange r >> repeat(size_t k = npos) -> ConditionalBidirectionalRange;
Container& c << repeat(size_t k = npos);
```

Repeats the input range `k` times.

```c++
ForwardRange r >> subsets(size_t k) -> InputNestedRange;
```

Generates all subsets of the input range, in ascending order of size; the
output contains <code>2<sup>n</sup></code> elements, each a collection of
`0-k` elements.

### Generation algorithms

_Generation algorithms create an output range with no input range_

```c++
epsilon<T>() -> RandomAccessRange;
```

Returns an empty range.

```c++
fill(T t, size_t k = npos) -> RandomAccessRange;
Range& r << fill(T t, size_t k = npos);
```

Returns a range containing `k` copies of `t`.

```c++
generate(GeneratorFunction f, size_t n = npos) -> InputRange;
Range& r << generate(GeneratorFunction f, size_t n = npos);
```

Returns a range of length `n`, generated by calling `f()` for each element.

```c++
iota(T init) -> ForwardRange;
iota(T init, UnaryFunction f, size_t n = npos) -> ForwardRange;
iota(T init, T delta, size_t n = npos) -> ForwardRange;
Range& r << iota(T init);
Range& r << iota(T init, UnaryFunction f, size_t n = npos);
Range& r << iota(T init, T delta, size_t n = npos);
```

Generate a sequence starting with the `init` value. The first version calls
`++x` on each element to generate the next one; the second calls `f(x)`; the
third calls `x+delta`.

```c++
random(RandomDistributon d, RandomEngine& rng, size_t n = npos)
    -> InputRange;
Range& r << random(RandomDistributon d, RandomEngine& rng, size_t n = npos);
```

Generate a sequence of random values, obtained by calling `d(rng)` for each
element.

single(T t) -> RandomAccessRange;

Returns a range containing a single element.

### Permutation algorithms

_Permutation algorithms return a permutation of the input range_

```c++
Range r >> next_permutation(ComparisonPredicate p = std::less)
    -> TemporaryRandomAccessRange;
Range r >> prev_permutation(ComparisonPredicate p = std::less)
    -> TemporaryRandomAccessRange;
BidirectionalRange& r << next_permutation(ComparisonPredicate p = std::less);
BidirectionalRange& r << prev_permutation(ComparisonPredicate p = std::less);
```

Return the next or previous permutation of the input range, in lexicographical
order.

```c++
BidirectionalRange r >> reverse -> BidirectionalRange2;
BidirectionalRange& r << reverse;
```

Reverse the input range.

```c++
Range r >> shuffle(RandomEngine& rng) -> TemporaryRandomAccessRange;
RandomAccessRange& r << shuffle(RandomEngine& rng);
```

Shuffle the input range into a random order.

```c++
Range r >> sort(ComparisonPredicate p = std::less)
    -> TemporaryRandomAccessRange;
Range r >> stable_sort(ComparisonPredicate p = std::less)
    -> TemporaryRandomAccessRange;
RandomAccessRange& r << sort(ComparisonPredicate p = std::less);
RandomAccessRange& r << stable_sort(ComparisonPredicate p = std::less);
```

Sort the input range according to the comparison predicate.

### Reduction algorithms

_Reduction algorithms take an input range and return a scalar result_

```c++
Range r >> all_of(UnaryPredicate p) -> bool;
Range r >> any_of(UnaryPredicate p) -> bool;
Range r >> none_of(UnaryPredicate p) -> bool;
```

True if the predicate returns true for all elements of the input range, for at
least one of them, or for none of them, respectively. If the range is empty,
`all_of()` and `none_of()` are both true, and `any_of()` is false.

```c++
Range r >> count -> size_t;
Range r >> count(T t) -> size_t;
Range r >> count_if(UnaryPredicate p) -> size_t;
```

The first version returns the length of the input range. The other versions
return the number of elements equal to the given value or matching the given
predicate.

```c++
Range r >> fold_left(T t, BinaryFunction f) -> T;
BidirectionalRange r >> fold_right(T t, BinaryFunction f) -> T;
Range r >> product(T init = 1) -> T;
Range r >> reduce(BinaryFunction f) -> T;
Range r >> sum(T init = T()) -> T;
```

The `fold_left()` and `fold_right()` algorithms perform a left to right or
right to left fold on the input range, starting with the given initial value
and calling `t=f(t,x)` or `t=f(x,t)`, respectively, for each element. The
`reduce()` algorithm performs a fold in unspecified order (possibly in
parallel); if the range is empty, `reduce()` returns a default constructed
object of its value type; if the range contains only one element; `reduce()`
returns that element without calling the function. The `sum()` and `product()`
algorithms are convenience functions for left folds using the addition or
multiplication operators.

```c++
Range r >> is_empty -> bool;
Range r >> is_nonempty -> bool;
```

True if the range is or is not empty.

```c++
Range r >> is_sorted(ComparisonPredicate p = std::less) -> bool;
```

True if the range is sorted according to the comparison predicate.

```c++
Range r >> max(ComparisonPredicate p = std::less) -> ValueType;
Range r >> min(ComparisonPredicate p = std::less) -> ValueType;
Range r >> min_max(ComparisonPredicate p = std::less)
    -> std::pair<ValueType>;
```

Return the maximum and minimum values in the range, according to the
comparison predicate.

### Selection algorithms

_Selection algorithms return a subset of the input range_

```c++
Range r >> after(T t) -> ConditionalForwardRange;
Range r >> after_if(UnaryPredicate p) -> ConditionalForwardRange;
Range r >> before(T t) -> ConditionalForwardRange;
Range r >> before_if(UnaryPredicate p) -> ConditionalForwardRange;
Range r >> from(T t) -> ConditionalForwardRange;
Range r >> from_if(UnaryPredicate p) -> ConditionalForwardRange;
Range r >> until(T t) -> ConditionalForwardRange;
Range r >> until_if(UnaryPredicate p) -> ConditionalForwardRange;
Container& c << after(T t);
Container& c << after_if(UnaryPredicate p);
Container& c << before(T t);
Container& c << before_if(UnaryPredicate p);
Container& c << from(T t);
Container& c << from_if(UnaryPredicate p);
Container& c << until(T t);
Container& c << until_if(UnaryPredicate p);
```

Return part of a range based on the first element matching the given value or
predicate. The `before[_if]()` algorithm returns all elements up to, but not
including, the first matching element; `until[_if]()` returns all elements up
to, and including, the first matching element; `from[_if]()` returns all
elements starting from, and including, the first matching element;
`after[_if]()` returns all elements after, but not including, the first
matching element.

```c++
Range r >> filter(UnaryPredicate p) -> ConditionalForwardRange;
Range r >> filter_out(UnaryPredicate p) -> ConditionalForwardRange;
Container& c << filter(UnaryPredicate p);
Container& c << filter_out(UnaryPredicate p);
```

Select all elements matching, or not matching, the predicate.

```c++
Range r >> not_null -> ConditionalForwardRange;
Container& c << not_null;
```

Filters out all elements elements for which `static_cast<bool>(x)` is false.

```c++
RandomAccessRange r >> sample_k(size_t k, RandomEngine& rng)
    -> TemporaryRandomAccessRange;
Range r >> sample_p(double p, RandomEngine& rng)
    -> ConditionalForwardRange;
RandomAccessRange r >> sample_replace(size_t k, RandomEngine& rng)
    -> TemporaryRandomAccessRange;
Container& c << sample_k(size_t k, RandomEngine& rng);
Container& c << sample_p(double p, RandomEngine& rng);
Container& c << sample_replace(size_t k, RandomEngine& rng);
```

These return various kinds of random sample of elements from the input range.
The `sample_k()` algorithm selects a sample of `k` elements; behaviour is
undefined if `k>n`. The `sample_p()` algorithm selects each element with
probability `p`; behaviour is undefined if `p<0` or `p>1`. The
`sample_replace()` algorithm selects a random element `k` times, with
replacement. For `sample_k()` and `sample_p()`, the elements in the output
range are in the same order as they were in the input. None of these
algorithms are guaranteed to produce identical output in different compilation
environments.

```c++
Range r >> remove(T t) -> ConditionalForwardRange;
Container& c << remove(T t);
```

Remove all elements equal to the argument.

```c++
Range r >> skip(size_t k) -> ConditionalForwardRange;
Range r >> take(size_t k) -> ConditionalForwardRange;
Container& c << skip(size_t k);
Container& c << take(size_t k);
```

Skip the first `k` elements, returning all elements after those, or return
only the first `k` elements. If `n<=k`, `skip()` returns nothing and `take()`
returns the input range unchanged.

```c++
Range r >> stride(size_t step) -> ConditionalForwardRange;
Range r >> stride(size_t skip, size_t step) -> ConditionalForwardRange;
Container& c << stride(size_t step);
Container& c << stride(size_t skip, size_t step);
```

Return one element for every `step` elements in the input range. Optionally,
the number of elements to skip before the first accepted element can be
specified; by default, the last element of each group is returned (equivalent
to `skip=step-1`).

```c++
Range r >> unique(EqualityPredicate p = std::equal_to)
    -> ConditionalForwardRange;
Container& c << unique(EqualityPredicate p = std::equal_to);
```

Return one element from each group of adjacent equal elements, according to
the equality predicate.

### Transformation algorithms

_Transformation algorithms apply a one-to-one mapping from the input range to the output_

```c++
Range r >> const_ptr<T>() -> Range2;
Range r >> dynamic_ptr<T>() -> Range2;
Range r >> reinterpret_ptr<T>() -> Range2;
Range r >> static_ptr<T>() -> Range2;
ForwardRange& r << const_ptr<T>();
ForwardRange& r << dynamic_ptr<T>();
ForwardRange& r << reinterpret_ptr<T>();
ForwardRange& r << static_ptr<T>();
```

These require the value type of the input range to be a pointer, and will
apply one of the standard cast operators (`const_cast`, `dynamic_cast`,
`reinterpret_cast`, or `static_cast`) to convert it to a `T*`. For all except
`reinterpret_ptr()`, these will also work with shared pointers.

```c++
Range r >> construct<T>() -> Range2;
Range r >> initialize<T>() -> Range2;
```

Each element of the output range is created by constructing a `T` using the
corresponding element of the input range to supply the constructor arguments;
if the input value type is a pair or tuple, its fields will be passed as
separate arguments. The two algorithms do the same thing, except that
`construct()` calls `T(args...)`, while `initialize()` calls `T{args...}`.

```c++
Range r >> convert<T>() -> Range2;
ForwardRange& r << convert<T>();
```

Output elements are generated by calling `static_cast<T>(x)` on the input
elements.

```c++
Range r >> dereference -> Range2;
```

Output elements are generated by calling `*x` on the input elements.

```c++
Range r >> indexed(int init = 0, int delta = 1)
    -> ConditionalForwardRange;
```

Each output element is a pair `(i,x)`, where `x` is the input element and `i`
is its index. Indexing starts from zero by default; optionally the initial
value and increment can be supplied.

```c++
Range r >> iterators -> Range2;
```

The output is the range of iterators from the input range.

```c++
Range r >> keys -> Range2;
Range r >> values -> Range2;
```

The input value type is expected to be a pair or pair-like type; the output is
the `first` (`keys()`) or `second` (`values()`) member from each input value.

```c++
Range r >> map(UnaryFunction f) -> Range2;
PairRange r >> map_pairs(BinaryFunction f) -> Range;
ForwardRange& r << map(UnaryFunction f);
```

Each output element is generated by calling `f(x)` on the input element, or
`f(x.first,x.second)` for `map_pairs()`.

```c++
Range r >> map_if(UnaryPredicate p, UnaryFunction f) -> Range2;
ForwardRange& r << map_if(UnaryPredicate p, UnaryFunction f);
```

Elements for which `p(x)` is true are replaced with `f(x)`; other elements are
passed through unchanged.

```c++
Range r >> replace(T1 t1, T2 t2) -> Range2;
Range r >> replace_if(UnaryPredicate p, T t2) -> Range2;
ForwardRange& r << replace(T1 t1, T2 t2);
ForwardRange& r << replace_if(UnaryPredicate p, T t2);
```

Input elements equal to `t1`, or matching the predicate, are replaced with
`t2`; other input elements are passed through unchanged.

```c++
PairRange r >> swap_pairs -> PairRange2;
PairContainer& c << swap_pairs;
```

Swaps the `first` and `second` member of each pair.
