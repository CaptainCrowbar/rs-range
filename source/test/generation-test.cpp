#include "rs-range/generation.hpp"
#include "rs-range/selection.hpp"
#include "rs-format/string.hpp"
#include "rs-tl/iterator.hpp"
#include "rs-unit-test.hpp"
#include <cmath>
#include <random>
#include <string>
#include <vector>

using namespace RS::Format;
using namespace RS::Range;
using namespace RS::TL;
using namespace RS::UnitTest;
using namespace std::literals;

namespace {

    template <typename Range>
    void stats(const Range& r, double& mean, double& sd) {
        double n = 0, sum = 0, sum2 = 0;
        for (auto& t: r) {
            auto x = double(t);
            ++n;
            sum += x;
            sum2 += x * x;
        }
        mean = sum / n;
        sd = std::sqrt(sum2 / n - mean * mean);
    }

}

void test_rs_range_generation_epsilon() {

    std::string s;

    TRY(epsilon<char> >> overwrite(s));                 TEST_EQUAL(s, "");
    TRY(epsilon<char> >> passthrough >> overwrite(s));  TEST_EQUAL(s, "");

}

void test_rs_range_generation_fill() {

    std::string s;

    TRY(fill('x', 0) >> overwrite(s));  TEST_EQUAL(s, "");
    TRY(fill('x', 1) >> overwrite(s));  TEST_EQUAL(s, "x");
    TRY(fill('x', 2) >> overwrite(s));  TEST_EQUAL(s, "xx");
    TRY(fill('x', 3) >> overwrite(s));  TEST_EQUAL(s, "xxx");
    TRY(fill('x', 4) >> overwrite(s));  TEST_EQUAL(s, "xxxx");
    TRY(fill('x', 5) >> overwrite(s));  TEST_EQUAL(s, "xxxxx");

    TRY(fill('x', 0) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "");
    TRY(fill('x', 1) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "x");
    TRY(fill('x', 2) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "xx");
    TRY(fill('x', 3) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "xxx");
    TRY(fill('x', 4) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "xxxx");
    TRY(fill('x', 5) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "xxxxx");

    s.resize(0);  TRY(s << fill('x'));  TEST_EQUAL(s, "");
    s.resize(1);  TRY(s << fill('x'));  TEST_EQUAL(s, "x");
    s.resize(2);  TRY(s << fill('x'));  TEST_EQUAL(s, "xx");
    s.resize(3);  TRY(s << fill('x'));  TEST_EQUAL(s, "xxx");
    s.resize(4);  TRY(s << fill('x'));  TEST_EQUAL(s, "xxxx");
    s.resize(5);  TRY(s << fill('x'));  TEST_EQUAL(s, "xxxxx");

}

void test_rs_range_generation_generate() {

    std::string s;
    int n;
    auto f = [&n] { return char('a' + n++); };

    n = 0;  TRY(generate(f, 0) >> overwrite(s));  TEST_EQUAL(s, "");
    n = 0;  TRY(generate(f, 1) >> overwrite(s));  TEST_EQUAL(s, "a");
    n = 0;  TRY(generate(f, 2) >> overwrite(s));  TEST_EQUAL(s, "ab");
    n = 0;  TRY(generate(f, 3) >> overwrite(s));  TEST_EQUAL(s, "abc");
    n = 0;  TRY(generate(f, 4) >> overwrite(s));  TEST_EQUAL(s, "abcd");
    n = 0;  TRY(generate(f, 5) >> overwrite(s));  TEST_EQUAL(s, "abcde");

    n = 0;  TRY(generate(f, 0) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "");
    n = 0;  TRY(generate(f, 1) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "a");
    n = 0;  TRY(generate(f, 2) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "ab");
    n = 0;  TRY(generate(f, 3) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "abc");
    n = 0;  TRY(generate(f, 4) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "abcd");
    n = 0;  TRY(generate(f, 5) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "abcde");

    n = 0;  s.resize(0);  TRY(s << generate(f));  TEST_EQUAL(s, "");
    n = 0;  s.resize(1);  TRY(s << generate(f));  TEST_EQUAL(s, "a");
    n = 0;  s.resize(2);  TRY(s << generate(f));  TEST_EQUAL(s, "ab");
    n = 0;  s.resize(3);  TRY(s << generate(f));  TEST_EQUAL(s, "abc");
    n = 0;  s.resize(4);  TRY(s << generate(f));  TEST_EQUAL(s, "abcd");
    n = 0;  s.resize(5);  TRY(s << generate(f));  TEST_EQUAL(s, "abcde");

}

void test_rs_range_generation_iota() {

    std::string s;
    auto f = [] (char c) { return ascii_isupper(c) ? ascii_tolower(c + 1) : ascii_toupper(c + 1); };

    TRY(iota('a') >> take(10) >> overwrite(s));   TEST_EQUAL(s, "abcdefghij");
    TRY(iota('a', char(2), 10) >> overwrite(s));  TEST_EQUAL(s, "acegikmoqs");
    TRY(iota('a', f, 10) >> overwrite(s));        TEST_EQUAL(s, "aBcDeFgHiJ");

    TRY(iota('a') >> passthrough >> take(10) >> overwrite(s));   TEST_EQUAL(s, "abcdefghij");
    TRY(iota('a', char(2), 10) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "acegikmoqs");
    TRY(iota('a', f, 10) >> passthrough >> overwrite(s));        TEST_EQUAL(s, "aBcDeFgHiJ");

    s.resize(10);

    TRY(s << iota('a'));           TEST_EQUAL(s, "abcdefghij");
    TRY(s << iota('a', char(2)));  TEST_EQUAL(s, "acegikmoqs");
    TRY(s << iota('a', f));        TEST_EQUAL(s, "aBcDeFgHiJ");

}

void test_rs_range_generation_random() {

    static constexpr size_t n = 100000;

    std::vector<int> v;
    std::uniform_int_distribution<int> uid(0, 100);
    std::mt19937 rng(42);
    double mean = 0, sd = 0;

    TRY(random(uid, rng, 0) >> overwrite(v));
    TEST_EQUAL(v.size(), 0u);

    TRY(random(uid, rng, n) >> overwrite(v));
    TEST_EQUAL(v.size(), n);
    TRY(stats(v, mean, sd));
    TEST_NEAR(mean, 50, 0.1);
    TEST_NEAR(sd, 29.15, 0.1);

    TRY(random(uid, rng, n) >> passthrough >> overwrite(v));
    TEST_EQUAL(v.size(), n);
    TRY(stats(v, mean, sd));
    TEST_NEAR(mean, 50, 0.1);
    TEST_NEAR(sd, 29.15, 0.1);

    v.clear();
    v.resize(n, 0);
    TRY(v << random(uid, rng, n));
    TEST_EQUAL(v.size(), n);
    TRY(stats(v, mean, sd));
    TEST_NEAR(mean, 50, 0.1);
    TEST_NEAR(sd, 29.15, 0.1);

}

void test_rs_range_generation_single() {

    std::string s;

    TRY(single('x') >> overwrite(s));                 TEST_EQUAL(s, "x");
    TRY(single('x') >> passthrough >> overwrite(s));  TEST_EQUAL(s, "x");

}
