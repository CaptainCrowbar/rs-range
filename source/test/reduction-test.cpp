#include "rs-range/reduction.hpp"
#include "rs-format/string.hpp"
#include "rs-tl/iterator.hpp"
#include "rs-unit-test.hpp"
#include <functional>
#include <string>
#include <utility>
#include <vector>

using namespace RS::Format;
using namespace RS::Range;
using namespace RS::TL;
using namespace RS::UnitTest;
using namespace std::literals;

void test_rs_range_reduction_all_any_none() {

    TEST(""s >> all_of(ascii_isprint));
    TEST("Hello world"s >> all_of(ascii_isprint));
    TEST(! ("Hello world"s >> all_of(ascii_isspace)));
    TEST(! ("Hello world"s >> all_of(ascii_iscntrl)));

    TEST(! (""s >> any_of(ascii_isprint)));
    TEST("Hello world"s >> any_of(ascii_isprint));
    TEST("Hello world"s >> any_of(ascii_isspace));
    TEST(! ("Hello world"s >> any_of(ascii_iscntrl)));

    TEST(""s >> none_of(ascii_isprint));
    TEST(! ("Hello world"s >> none_of(ascii_isprint)));
    TEST(! ("Hello world"s >> none_of(ascii_isspace)));
    TEST("Hello world"s >> none_of(ascii_iscntrl));

}

void test_rs_range_reduction_count() {

    size_t n = 0;

    TRY(n = "Hello world"s >> count);                    TEST_EQUAL(n, 11u);
    TRY(n = "Hello world"s >> count('l'));               TEST_EQUAL(n, 3u);
    TRY(n = "Hello world"s >> count('*'));               TEST_EQUAL(n, 0u);
    TRY(n = "Hello world"s >> count_if(ascii_isalpha));  TEST_EQUAL(n, 10u);
    TRY(n = "Hello world"s >> count_if(ascii_isspace));  TEST_EQUAL(n, 1u);
    TRY(n = "Hello world"s >> count_if(ascii_ispunct));  TEST_EQUAL(n, 0u);

    TRY(n = "Hello world"s >> passthrough >> count);                    TEST_EQUAL(n, 11u);
    TRY(n = "Hello world"s >> passthrough >> count('l'));               TEST_EQUAL(n, 3u);
    TRY(n = "Hello world"s >> passthrough >> count('*'));               TEST_EQUAL(n, 0u);
    TRY(n = "Hello world"s >> passthrough >> count_if(ascii_isalpha));  TEST_EQUAL(n, 10u);
    TRY(n = "Hello world"s >> passthrough >> count_if(ascii_isspace));  TEST_EQUAL(n, 1u);
    TRY(n = "Hello world"s >> passthrough >> count_if(ascii_ispunct));  TEST_EQUAL(n, 0u);

    TRY(n = "Hello world"s >> (passthrough * count));                    TEST_EQUAL(n, 11u);
    TRY(n = "Hello world"s >> (passthrough * count('l')));               TEST_EQUAL(n, 3u);
    TRY(n = "Hello world"s >> (passthrough * count('*')));               TEST_EQUAL(n, 0u);
    TRY(n = "Hello world"s >> (passthrough * count_if(ascii_isalpha)));  TEST_EQUAL(n, 10u);
    TRY(n = "Hello world"s >> (passthrough * count_if(ascii_isspace)));  TEST_EQUAL(n, 1u);
    TRY(n = "Hello world"s >> (passthrough * count_if(ascii_ispunct)));  TEST_EQUAL(n, 0u);

}

void test_rs_range_reduction_fold() {

    std::vector<int> v = {1,2,3,4,5};
    int n = 0;
    auto f = [] (int x, int y) { return 2 * x + y; };

    TRY(n = v >> fold_left(0, std::plus<>()));                  TEST_EQUAL(n, 15);
    TRY(n = v >> fold_right(0, std::plus<>()));                 TEST_EQUAL(n, 15);
    TRY(n = v >> fold_left(0, f));                              TEST_EQUAL(n, 57);
    TRY(n = v >> fold_right(0, f));                             TEST_EQUAL(n, 30);
    TRY(n = v >> passthrough >> fold_left(0, std::plus<>()));   TEST_EQUAL(n, 15);
    TRY(n = v >> (passthrough * fold_left(0, std::plus<>())));  TEST_EQUAL(n, 15);
    TRY(n = v >> passthrough >> fold_left(0, f));               TEST_EQUAL(n, 57);
    TRY(n = v >> (passthrough * fold_left(0, f)));              TEST_EQUAL(n, 57);

}

void test_rs_range_reduction_is_empty() {

    std::string s;

    s = "";             TEST(s >> is_empty);      TEST(! (s >> is_nonempty));
    s = "x";            TEST(! (s >> is_empty));  TEST(s >> is_nonempty);
    s = "hello world";  TEST(! (s >> is_empty));  TEST(s >> is_nonempty);

}

void test_rs_range_reduction_is_sorted() {

    std::string s;

    s = "";        TEST(s >> is_sorted);      TEST(s >> is_sorted(std::greater<>()));
    s = "a";       TEST(s >> is_sorted);      TEST(s >> is_sorted(std::greater<>()));
    s = "az";      TEST(s >> is_sorted);      TEST(! (s >> is_sorted(std::greater<>())));
    s = "za";      TEST(! (s >> is_sorted));  TEST(s >> is_sorted(std::greater<>()));
    s = "aeiou";   TEST(s >> is_sorted);      TEST(! (s >> is_sorted(std::greater<>())));
    s = "aeioua";  TEST(! (s >> is_sorted));  TEST(! (s >> is_sorted(std::greater<>())));
    s = "uoiea";   TEST(! (s >> is_sorted));  TEST(s >> is_sorted(std::greater<>()));

}

void test_rs_range_reduction_min_max() {

    std::string s0, s1 = "Hello world";
    char c = 0;
    std::pair<char, char> cp;

    TRY(c = s0 >> min);                         TEST_EQUAL(c, '\0');
    TRY(c = s0 >> max);                         TEST_EQUAL(c, '\0');
    TRY(cp = s0 >> min_max);                    TEST_EQUAL(cp.first, '\0'); TEST_EQUAL(cp.second, '\0');
    TRY(c = s1 >> min);                         TEST_EQUAL(c, ' ');
    TRY(c = s1 >> max);                         TEST_EQUAL(c, 'w');
    TRY(cp = s1 >> min_max);                    TEST_EQUAL(cp.first, ' '); TEST_EQUAL(cp.second, 'w');
    TRY(c = s1 >> min(std::greater<>()));       TEST_EQUAL(c, 'w');
    TRY(c = s1 >> max(std::greater<>()));       TEST_EQUAL(c, ' ');
    TRY(cp = s1 >> min_max(std::greater<>()));  TEST_EQUAL(cp.first, 'w'); TEST_EQUAL(cp.second, ' ');

}

void test_rs_range_reduction_reduce() {

    std::vector<int> v0, v1 = {42}, v5 = {1,2,3,4,5};
    int n = 0;
    auto f = [] (int x, int y) { return 2 * x + y; };

    TRY(n = v0 >> reduce(std::plus<>()));                  TEST_EQUAL(n, 0);
    TRY(n = v0 >> reduce(f));                              TEST_EQUAL(n, 0);
    TRY(n = v1 >> reduce(std::plus<>()));                  TEST_EQUAL(n, 42);
    TRY(n = v1 >> reduce(f));                              TEST_EQUAL(n, 42);
    TRY(n = v5 >> reduce(std::plus<>()));                  TEST_EQUAL(n, 15);
    TRY(n = v5 >> reduce(f));                              TEST_EQUAL(n, 57);
    TRY(n = v5 >> passthrough >> reduce(std::plus<>()));   TEST_EQUAL(n, 15);
    TRY(n = v5 >> (passthrough * reduce(std::plus<>())));  TEST_EQUAL(n, 15);
    TRY(n = v5 >> passthrough >> reduce(f));               TEST_EQUAL(n, 57);
    TRY(n = v5 >> (passthrough * reduce(f)));              TEST_EQUAL(n, 57);

}

void test_rs_range_reduction_sum_product() {

    std::vector<int> v0, v1 = {1,2,3,4,5};
    int x = 0;

    TRY(x = v0 >> sum);                       TEST_EQUAL(x, 0);
    TRY(x = v0 >> sum(100));                  TEST_EQUAL(x, 100);
    TRY(x = v1 >> sum);                       TEST_EQUAL(x, 15);
    TRY(x = v1 >> sum(100));                  TEST_EQUAL(x, 115);
    TRY(x = v0 >> passthrough >> sum);        TEST_EQUAL(x, 0);
    TRY(x = v0 >> passthrough >> sum(100));   TEST_EQUAL(x, 100);
    TRY(x = v1 >> passthrough >> sum);        TEST_EQUAL(x, 15);
    TRY(x = v1 >> passthrough >> sum(100));   TEST_EQUAL(x, 115);
    TRY(x = v0 >> (passthrough * sum));       TEST_EQUAL(x, 0);
    TRY(x = v0 >> (passthrough * sum(100)));  TEST_EQUAL(x, 100);
    TRY(x = v1 >> (passthrough * sum));       TEST_EQUAL(x, 15);
    TRY(x = v1 >> (passthrough * sum(100)));  TEST_EQUAL(x, 115);

    TRY(x = v0 >> product);                       TEST_EQUAL(x, 1);
    TRY(x = v0 >> product(100));                  TEST_EQUAL(x, 100);
    TRY(x = v1 >> product);                       TEST_EQUAL(x, 120);
    TRY(x = v1 >> product(100));                  TEST_EQUAL(x, 12000);
    TRY(x = v0 >> passthrough >> product);        TEST_EQUAL(x, 1);
    TRY(x = v0 >> passthrough >> product(100));   TEST_EQUAL(x, 100);
    TRY(x = v1 >> passthrough >> product);        TEST_EQUAL(x, 120);
    TRY(x = v1 >> passthrough >> product(100));   TEST_EQUAL(x, 12000);
    TRY(x = v0 >> (passthrough * product));       TEST_EQUAL(x, 1);
    TRY(x = v0 >> (passthrough * product(100)));  TEST_EQUAL(x, 100);
    TRY(x = v1 >> (passthrough * product));       TEST_EQUAL(x, 120);
    TRY(x = v1 >> (passthrough * product(100)));  TEST_EQUAL(x, 12000);

}
