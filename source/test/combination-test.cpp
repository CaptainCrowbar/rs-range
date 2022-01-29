#include "rs-range/combination.hpp"
#include "rs-tl/iterator.hpp"
#include "rs-unit-test.hpp"
#include <functional>
#include <map>
#include <string>
#include <vector>

using namespace RS::Range;
using namespace RS::TL;
using namespace RS::UnitTest;
using namespace std::literals;

void test_rs_range_combination_compare() {

    std::string s1, s2;
    int rc = 0;

    s1 = "";       s2 = "";             TEST(! (s1 >> compare(s2)));  TEST(! (s2 >> compare(s1)));
    s1 = "";       s2 = "hello";        TEST(s1 >> compare(s2));      TEST(! (s2 >> compare(s1)));
    s1 = "hello";  s2 = "hello";        TEST(! (s1 >> compare(s2)));  TEST(! (s2 >> compare(s1)));
    s1 = "hello";  s2 = "world";        TEST(s1 >> compare(s2));      TEST(! (s2 >> compare(s1)));
    s1 = "hello";  s2 = "hello world";  TEST(s1 >> compare(s2));      TEST(! (s2 >> compare(s1)));

    s1 = "";       s2 = "";             TRY(rc = s1 >> compare_3way(s2));  TEST_EQUAL(rc, 0);   TRY(rc = s2 >> compare_3way(s1));  TEST_EQUAL(rc, 0);
    s1 = "";       s2 = "hello";        TRY(rc = s1 >> compare_3way(s2));  TEST_EQUAL(rc, -1);  TRY(rc = s2 >> compare_3way(s1));  TEST_EQUAL(rc, 1);
    s1 = "hello";  s2 = "hello";        TRY(rc = s1 >> compare_3way(s2));  TEST_EQUAL(rc, 0);   TRY(rc = s2 >> compare_3way(s1));  TEST_EQUAL(rc, 0);
    s1 = "hello";  s2 = "world";        TRY(rc = s1 >> compare_3way(s2));  TEST_EQUAL(rc, -1);  TRY(rc = s2 >> compare_3way(s1));  TEST_EQUAL(rc, 1);
    s1 = "hello";  s2 = "hello world";  TRY(rc = s1 >> compare_3way(s2));  TEST_EQUAL(rc, -1);  TRY(rc = s2 >> compare_3way(s1));  TEST_EQUAL(rc, 1);

    s1 = "";       s2 = "";             TEST(s1 >> is_equal(s2));      TEST(s2 >> is_equal(s1));
    s1 = "";       s2 = "hello";        TEST(! (s1 >> is_equal(s2)));  TEST(! (s2 >> is_equal(s1)));
    s1 = "hello";  s2 = "hello";        TEST(s1 >> is_equal(s2));      TEST(s2 >> is_equal(s1));
    s1 = "hello";  s2 = "world";        TEST(! (s1 >> is_equal(s2)));  TEST(! (s2 >> is_equal(s1)));
    s1 = "hello";  s2 = "hello world";  TEST(! (s1 >> is_equal(s2)));  TEST(! (s2 >> is_equal(s1)));

    s1 = "";       s2 = "";             TEST(s1 >> is_equivalent(s2));      TEST(s2 >> is_equivalent(s1));
    s1 = "";       s2 = "hello";        TEST(! (s1 >> is_equivalent(s2)));  TEST(! (s2 >> is_equivalent(s1)));
    s1 = "hello";  s2 = "hello";        TEST(s1 >> is_equivalent(s2));      TEST(s2 >> is_equivalent(s1));
    s1 = "hello";  s2 = "world";        TEST(! (s1 >> is_equivalent(s2)));  TEST(! (s2 >> is_equivalent(s1)));
    s1 = "hello";  s2 = "hello world";  TEST(! (s1 >> is_equivalent(s2)));  TEST(! (s2 >> is_equivalent(s1)));

}

void test_rs_range_combination_concat() {

    std::string s1 = "Hello", s2 = "World", t;

    TRY(s1 >> concat(s2) >> overwrite(t));  TEST_EQUAL(t, "HelloWorld");
    TRY(s2 >> prefix(s1) >> overwrite(t));  TEST_EQUAL(t, "HelloWorld");

    TRY(s1 >> passthrough >> concat(s2) >> passthrough >> overwrite(t));  TEST_EQUAL(t, "HelloWorld");
    TRY(s2 >> passthrough >> prefix(s1) >> passthrough >> overwrite(t));  TEST_EQUAL(t, "HelloWorld");

    t = s1;  TRY(t << concat(s2));  TEST_EQUAL(t, "HelloWorld");
    t = s2;  TRY(t << prefix(s1));  TEST_EQUAL(t, "HelloWorld");

    TRY(s1 >> passthrough * concat(s2) >> overwrite(t));                TEST_EQUAL(t, "HelloWorld");
    TRY(s1 >> concat(s2) * passthrough >> overwrite(t));                TEST_EQUAL(t, "HelloWorld");
    TRY(s1 >> passthrough * concat(s2) * passthrough >> overwrite(t));  TEST_EQUAL(t, "HelloWorld");
    TRY(s2 >> passthrough * prefix(s1) >> overwrite(t));                TEST_EQUAL(t, "HelloWorld");
    TRY(s2 >> prefix(s1) * passthrough >> overwrite(t));                TEST_EQUAL(t, "HelloWorld");
    TRY(s2 >> passthrough * prefix(s1) * passthrough >> overwrite(t));  TEST_EQUAL(t, "HelloWorld");

}

void test_rs_range_combination_inner_product() {

    std::vector<int> v1 = {1,2,3,4,5,6,7,8,9,10}, v2 = {2,3,5,7,11};
    int n = 0;
    auto f1 = [] (int x, int y) { return x * y * y; };
    auto f2 = [] (int x, int y) { return 2 * x + y; };

    TRY(n = v1 >> inner_product(v2));                             TEST_EQUAL(n, 106);
    TRY(n = v2 >> inner_product(v1));                             TEST_EQUAL(n, 106);
    TRY(n = v1 >> inner_product(v2, 42, f1, f2));                 TEST_EQUAL(n, 2849);
    TRY(n = v1 >> passthrough >> inner_product(v2));              TEST_EQUAL(n, 106);
    TRY(n = v2 >> passthrough >> inner_product(v1));              TEST_EQUAL(n, 106);
    TRY(n = v1 >> passthrough >> inner_product(v2, 42, f1, f2));  TEST_EQUAL(n, 2849);
    TRY(n = v1 >> (passthrough * inner_product(v2)));             TEST_EQUAL(n, 106);
    TRY(n = v2 >> (passthrough * inner_product(v1)));             TEST_EQUAL(n, 106);

}

void test_rs_range_combination_interleave() {

    std::string s1, s2, s3;

    s1 = "";             s2 = "";             TRY(s1 >> interleave(s2) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "hello";        s2 = "";             TRY(s1 >> interleave(s2) >> overwrite(s3));  TEST_EQUAL(s3, "hello");
    s1 = "";             s2 = "world";        TRY(s1 >> interleave(s2) >> overwrite(s3));  TEST_EQUAL(s3, "world");
    s1 = "hello";        s2 = "world";        TRY(s1 >> interleave(s2) >> overwrite(s3));  TEST_EQUAL(s3, "hweolrllod");
    s1 = "hello world";  s2 = "goodbye";      TRY(s1 >> interleave(s2) >> overwrite(s3));  TEST_EQUAL(s3, "hgeololdob yweorld");
    s1 = "goodbye";      s2 = "hello world";  TRY(s1 >> interleave(s2) >> overwrite(s3));  TEST_EQUAL(s3, "ghoeoldlboy eworld");

    s1 = "";             s2 = "";             TRY(s1 >> passthrough >> interleave(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "hello";        s2 = "";             TRY(s1 >> passthrough >> interleave(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "hello");
    s1 = "";             s2 = "world";        TRY(s1 >> passthrough >> interleave(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "world");
    s1 = "hello";        s2 = "world";        TRY(s1 >> passthrough >> interleave(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "hweolrllod");
    s1 = "hello world";  s2 = "goodbye";      TRY(s1 >> passthrough >> interleave(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "hgeololdob yweorld");
    s1 = "goodbye";      s2 = "hello world";  TRY(s1 >> passthrough >> interleave(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "ghoeoldlboy eworld");

    s1 = "";             s2 = "";             TRY(s1 << interleave(s2));  TEST_EQUAL(s1, "");
    s1 = "hello";        s2 = "";             TRY(s1 << interleave(s2));  TEST_EQUAL(s1, "hello");
    s1 = "";             s2 = "world";        TRY(s1 << interleave(s2));  TEST_EQUAL(s1, "world");
    s1 = "hello";        s2 = "world";        TRY(s1 << interleave(s2));  TEST_EQUAL(s1, "hweolrllod");
    s1 = "hello world";  s2 = "goodbye";      TRY(s1 << interleave(s2));  TEST_EQUAL(s1, "hgeololdob yweorld");
    s1 = "goodbye";      s2 = "hello world";  TRY(s1 << interleave(s2));  TEST_EQUAL(s1, "ghoeoldlboy eworld");

    s1 = "hello";
    s2 = "world";

    TRY(s1 >> passthrough * interleave(s2) >> overwrite(s3));                TEST_EQUAL(s3, "hweolrllod");
    TRY(s1 >> interleave(s2) * passthrough >> overwrite(s3));                TEST_EQUAL(s3, "hweolrllod");
    TRY(s1 >> passthrough * interleave(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "hweolrllod");

}

void test_rs_range_combination_merge() {

    std::string s1, s2, s3;

    s1 = "";        s2 = "";        TRY(s1 >> merge(s2) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> merge(s2) >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> merge(s2) >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> merge(s2) >> overwrite(s3));  TEST_EQUAL(s3, "aabcdeefiou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> merge(s2) >> overwrite(s3));  TEST_EQUAL(s3, "aabcdeefiou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> merge(s2) >> overwrite(s3));  TEST_EQUAL(s3, "aeiouuvwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> merge(s2) >> overwrite(s3));  TEST_EQUAL(s3, "aeiouuvwxyz");

    s1 = "";        s2 = "";        TRY(s1 >> merge(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "fedcba";  s2 = "";        TRY(s1 >> merge(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "fedcba");
    s1 = "";        s2 = "fedcba";  TRY(s1 >> merge(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "fedcba");
    s1 = "fedcba";  s2 = "uoiea";   TRY(s1 >> merge(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "uoifeedcbaa");
    s1 = "uoiea";   s2 = "fedcba";  TRY(s1 >> merge(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "uoifeedcbaa");
    s1 = "zyxwvu";  s2 = "uoiea";   TRY(s1 >> merge(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "zyxwvuuoiea");
    s1 = "uoiea";   s2 = "zyxwvu";  TRY(s1 >> merge(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "zyxwvuuoiea");

    s1 = "";        s2 = "";        TRY(s1 >> passthrough >> merge(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> passthrough >> merge(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> passthrough >> merge(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> passthrough >> merge(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "aabcdeefiou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> passthrough >> merge(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "aabcdeefiou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> passthrough >> merge(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "aeiouuvwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> passthrough >> merge(s2) >> passthrough >> overwrite(s3));  TEST_EQUAL(s3, "aeiouuvwxyz");

    s1 = "";        s2 = "";        TRY(s1 << merge(s2));  TEST_EQUAL(s1, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 << merge(s2));  TEST_EQUAL(s1, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 << merge(s2));  TEST_EQUAL(s1, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 << merge(s2));  TEST_EQUAL(s1, "aabcdeefiou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 << merge(s2));  TEST_EQUAL(s1, "aabcdeefiou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 << merge(s2));  TEST_EQUAL(s1, "aeiouuvwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 << merge(s2));  TEST_EQUAL(s1, "aeiouuvwxyz");

    s1 = "abcdef";
    s2 = "aeiou";

    TRY(s1 >> passthrough * merge(s2) >> overwrite(s3));                TEST_EQUAL(s3, "aabcdeefiou");
    TRY(s1 >> merge(s2) * passthrough >> overwrite(s3));                TEST_EQUAL(s3, "aabcdeefiou");
    TRY(s1 >> passthrough * merge(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "aabcdeefiou");

}

void test_rs_range_combination_outer_product() {

    std::string s1 = "abc", s2 = "wxyz", t;
    std::multimap<char, char> m;
    std::vector<std::string> v;
    auto f = [] (char a, char b) { return std::string(2, a) + '/' + b; };

    TRY(s1 >> outer_product(s2) >> overwrite(m));                                   TEST_EQUAL(format_map(m), "{a:w,a:x,a:y,a:z,b:w,b:x,b:y,b:z,c:w,c:x,c:y,c:z}");
    TRY(s1 >> passthrough >> outer_product(s2) >> passthrough >> overwrite(m));     TEST_EQUAL(format_map(m), "{a:w,a:x,a:y,a:z,b:w,b:x,b:y,b:z,c:w,c:x,c:y,c:z}");
    TRY(s1 >> outer_product(s2, f) >> overwrite(v));                                TEST_EQUAL(format_range(v), "[aa/w,aa/x,aa/y,aa/z,bb/w,bb/x,bb/y,bb/z,cc/w,cc/x,cc/y,cc/z]");
    TRY(s1 >> passthrough >> outer_product(s2, f) >> passthrough >> overwrite(v));  TEST_EQUAL(format_range(v), "[aa/w,aa/x,aa/y,aa/z,bb/w,bb/x,bb/y,bb/z,cc/w,cc/x,cc/y,cc/z]");
    TRY(s1 >> passthrough * outer_product(s2) >> overwrite(m));                     TEST_EQUAL(format_map(m), "{a:w,a:x,a:y,a:z,b:w,b:x,b:y,b:z,c:w,c:x,c:y,c:z}");
    TRY(s1 >> outer_product(s2) * passthrough >> overwrite(m));                     TEST_EQUAL(format_map(m), "{a:w,a:x,a:y,a:z,b:w,b:x,b:y,b:z,c:w,c:x,c:y,c:z}");
    TRY(s1 >> passthrough * outer_product(s2) * passthrough >> overwrite(m));       TEST_EQUAL(format_map(m), "{a:w,a:x,a:y,a:z,b:w,b:x,b:y,b:z,c:w,c:x,c:y,c:z}");

    TRY(s1 >> self_cross >> overwrite(m));                                   TEST_EQUAL(format_map(m), "{a:a,a:b,a:c,b:a,b:b,b:c,c:a,c:b,c:c}");
    TRY(s1 >> passthrough >> self_cross >> passthrough >> overwrite(m));     TEST_EQUAL(format_map(m), "{a:a,a:b,a:c,b:a,b:b,b:c,c:a,c:b,c:c}");
    TRY(s1 >> self_cross(f) >> overwrite(v));                                TEST_EQUAL(format_range(v), "[aa/a,aa/b,aa/c,bb/a,bb/b,bb/c,cc/a,cc/b,cc/c]");
    TRY(s1 >> passthrough >> self_cross(f) >> passthrough >> overwrite(v));  TEST_EQUAL(format_range(v), "[aa/a,aa/b,aa/c,bb/a,bb/b,bb/c,cc/a,cc/b,cc/c]");
    TRY(s1 >> passthrough * self_cross >> overwrite(m));                     TEST_EQUAL(format_map(m), "{a:a,a:b,a:c,b:a,b:b,b:c,c:a,c:b,c:c}");
    TRY(s1 >> self_cross * passthrough >> overwrite(m));                     TEST_EQUAL(format_map(m), "{a:a,a:b,a:c,b:a,b:b,b:c,c:a,c:b,c:c}");
    TRY(s1 >> passthrough * self_cross * passthrough >> overwrite(m));       TEST_EQUAL(format_map(m), "{a:a,a:b,a:c,b:a,b:b,b:c,c:a,c:b,c:c}");

}

void test_rs_range_combination_set_difference() {

    std::string s1, s2, s3;

    s1 = "";        s2 = "";        TRY(s1 >> set_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> set_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> set_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> set_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "bcdf");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> set_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "iou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> set_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "vwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> set_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "aeio");

    s1 = "";        s2 = "";        TRY(s1 >> passthrough * set_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> passthrough * set_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> passthrough * set_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> passthrough * set_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "bcdf");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> passthrough * set_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "iou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> passthrough * set_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "vwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> passthrough * set_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "aeio");

    s1 = "";        s2 = "";        TRY(s1 >> set_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "fedcba";  s2 = "";        TRY(s1 >> set_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "fedcba");
    s1 = "";        s2 = "fedcba";  TRY(s1 >> set_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "fedcba";  s2 = "uoiea";   TRY(s1 >> set_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "fdcb");
    s1 = "uoiea";   s2 = "fedcba";  TRY(s1 >> set_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "uoi");
    s1 = "zyxwvu";  s2 = "uoiea";   TRY(s1 >> set_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "zyxwv");
    s1 = "uoiea";   s2 = "zyxwvu";  TRY(s1 >> set_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "oiea");

    s1 = "";        s2 = "";        TRY(s1 << set_difference(s2));  TEST_EQUAL(s1, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 << set_difference(s2));  TEST_EQUAL(s1, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 << set_difference(s2));  TEST_EQUAL(s1, "");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 << set_difference(s2));  TEST_EQUAL(s1, "bcdf");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 << set_difference(s2));  TEST_EQUAL(s1, "iou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 << set_difference(s2));  TEST_EQUAL(s1, "vwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 << set_difference(s2));  TEST_EQUAL(s1, "aeio");

}

void test_rs_range_combination_set_difference_from() {

    std::string s1, s2, s3;

    s1 = "";        s2 = "";        TRY(s1 >> set_difference_from(s2) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> set_difference_from(s2) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> set_difference_from(s2) >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> set_difference_from(s2) >> overwrite(s3));  TEST_EQUAL(s3, "iou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> set_difference_from(s2) >> overwrite(s3));  TEST_EQUAL(s3, "bcdf");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> set_difference_from(s2) >> overwrite(s3));  TEST_EQUAL(s3, "aeio");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> set_difference_from(s2) >> overwrite(s3));  TEST_EQUAL(s3, "vwxyz");

    s1 = "";        s2 = "";        TRY(s1 >> passthrough * set_difference_from(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> passthrough * set_difference_from(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> passthrough * set_difference_from(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> passthrough * set_difference_from(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "iou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> passthrough * set_difference_from(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "bcdf");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> passthrough * set_difference_from(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "aeio");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> passthrough * set_difference_from(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "vwxyz");

    s1 = "";        s2 = "";        TRY(s1 >> set_difference_from(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "fedcba";  s2 = "";        TRY(s1 >> set_difference_from(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "";        s2 = "fedcba";  TRY(s1 >> set_difference_from(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "fedcba");
    s1 = "fedcba";  s2 = "uoiea";   TRY(s1 >> set_difference_from(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "uoi");
    s1 = "uoiea";   s2 = "fedcba";  TRY(s1 >> set_difference_from(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "fdcb");
    s1 = "zyxwvu";  s2 = "uoiea";   TRY(s1 >> set_difference_from(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "oiea");
    s1 = "uoiea";   s2 = "zyxwvu";  TRY(s1 >> set_difference_from(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "zyxwv");

    s1 = "";        s2 = "";        TRY(s1 << set_difference_from(s2));  TEST_EQUAL(s1, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 << set_difference_from(s2));  TEST_EQUAL(s1, "");
    s1 = "";        s2 = "abcdef";  TRY(s1 << set_difference_from(s2));  TEST_EQUAL(s1, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 << set_difference_from(s2));  TEST_EQUAL(s1, "iou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 << set_difference_from(s2));  TEST_EQUAL(s1, "bcdf");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 << set_difference_from(s2));  TEST_EQUAL(s1, "aeio");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 << set_difference_from(s2));  TEST_EQUAL(s1, "vwxyz");

}

void test_rs_range_combination_set_intersection() {

    std::string s1, s2, s3;

    s1 = "";        s2 = "";        TRY(s1 >> set_intersection(s2) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> set_intersection(s2) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> set_intersection(s2) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> set_intersection(s2) >> overwrite(s3));  TEST_EQUAL(s3, "ae");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> set_intersection(s2) >> overwrite(s3));  TEST_EQUAL(s3, "ae");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> set_intersection(s2) >> overwrite(s3));  TEST_EQUAL(s3, "u");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> set_intersection(s2) >> overwrite(s3));  TEST_EQUAL(s3, "u");

    s1 = "";        s2 = "";        TRY(s1 >> passthrough * set_intersection(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> passthrough * set_intersection(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> passthrough * set_intersection(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> passthrough * set_intersection(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "ae");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> passthrough * set_intersection(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "ae");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> passthrough * set_intersection(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "u");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> passthrough * set_intersection(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "u");

    s1 = "";        s2 = "";        TRY(s1 >> set_intersection(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "fedcba";  s2 = "";        TRY(s1 >> set_intersection(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "";        s2 = "fedcba";  TRY(s1 >> set_intersection(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "fedcba";  s2 = "uoiea";   TRY(s1 >> set_intersection(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "ea");
    s1 = "uoiea";   s2 = "fedcba";  TRY(s1 >> set_intersection(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "ea");
    s1 = "zyxwvu";  s2 = "uoiea";   TRY(s1 >> set_intersection(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "u");
    s1 = "uoiea";   s2 = "zyxwvu";  TRY(s1 >> set_intersection(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "u");

    s1 = "";        s2 = "";        TRY(s1 << set_intersection(s2));  TEST_EQUAL(s1, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 << set_intersection(s2));  TEST_EQUAL(s1, "");
    s1 = "";        s2 = "abcdef";  TRY(s1 << set_intersection(s2));  TEST_EQUAL(s1, "");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 << set_intersection(s2));  TEST_EQUAL(s1, "ae");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 << set_intersection(s2));  TEST_EQUAL(s1, "ae");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 << set_intersection(s2));  TEST_EQUAL(s1, "u");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 << set_intersection(s2));  TEST_EQUAL(s1, "u");

}

void test_rs_range_combination_set_symmetric_difference() {

    std::string s1, s2, s3;

    s1 = "";        s2 = "";        TRY(s1 >> set_symmetric_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> set_symmetric_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> set_symmetric_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> set_symmetric_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "bcdfiou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> set_symmetric_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "bcdfiou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> set_symmetric_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "aeiovwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> set_symmetric_difference(s2) >> overwrite(s3));  TEST_EQUAL(s3, "aeiovwxyz");

    s1 = "";        s2 = "";        TRY(s1 >> passthrough * set_symmetric_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> passthrough * set_symmetric_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> passthrough * set_symmetric_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> passthrough * set_symmetric_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "bcdfiou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> passthrough * set_symmetric_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "bcdfiou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> passthrough * set_symmetric_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "aeiovwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> passthrough * set_symmetric_difference(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "aeiovwxyz");

    s1 = "";        s2 = "";        TRY(s1 >> set_symmetric_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "fedcba";  s2 = "";        TRY(s1 >> set_symmetric_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "fedcba");
    s1 = "";        s2 = "fedcba";  TRY(s1 >> set_symmetric_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "fedcba");
    s1 = "fedcba";  s2 = "uoiea";   TRY(s1 >> set_symmetric_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "uoifdcb");
    s1 = "uoiea";   s2 = "fedcba";  TRY(s1 >> set_symmetric_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "uoifdcb");
    s1 = "zyxwvu";  s2 = "uoiea";   TRY(s1 >> set_symmetric_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "zyxwvoiea");
    s1 = "uoiea";   s2 = "zyxwvu";  TRY(s1 >> set_symmetric_difference(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "zyxwvoiea");

    s1 = "";        s2 = "";        TRY(s1 << set_symmetric_difference(s2));  TEST_EQUAL(s1, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 << set_symmetric_difference(s2));  TEST_EQUAL(s1, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 << set_symmetric_difference(s2));  TEST_EQUAL(s1, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 << set_symmetric_difference(s2));  TEST_EQUAL(s1, "bcdfiou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 << set_symmetric_difference(s2));  TEST_EQUAL(s1, "bcdfiou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 << set_symmetric_difference(s2));  TEST_EQUAL(s1, "aeiovwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 << set_symmetric_difference(s2));  TEST_EQUAL(s1, "aeiovwxyz");

}

void test_rs_range_combination_set_union() {

    std::string s1, s2, s3;

    s1 = "";        s2 = "";        TRY(s1 >> set_union(s2) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> set_union(s2) >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> set_union(s2) >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> set_union(s2) >> overwrite(s3));  TEST_EQUAL(s3, "abcdefiou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> set_union(s2) >> overwrite(s3));  TEST_EQUAL(s3, "abcdefiou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> set_union(s2) >> overwrite(s3));  TEST_EQUAL(s3, "aeiouvwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> set_union(s2) >> overwrite(s3));  TEST_EQUAL(s3, "aeiouvwxyz");

    s1 = "";        s2 = "";        TRY(s1 >> passthrough * set_union(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 >> passthrough * set_union(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 >> passthrough * set_union(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 >> passthrough * set_union(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "abcdefiou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 >> passthrough * set_union(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "abcdefiou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 >> passthrough * set_union(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "aeiouvwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 >> passthrough * set_union(s2) * passthrough >> overwrite(s3));  TEST_EQUAL(s3, "aeiouvwxyz");

    s1 = "";        s2 = "";        TRY(s1 >> set_union(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "");
    s1 = "fedcba";  s2 = "";        TRY(s1 >> set_union(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "fedcba");
    s1 = "";        s2 = "fedcba";  TRY(s1 >> set_union(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "fedcba");
    s1 = "fedcba";  s2 = "uoiea";   TRY(s1 >> set_union(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "uoifedcba");
    s1 = "uoiea";   s2 = "fedcba";  TRY(s1 >> set_union(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "uoifedcba");
    s1 = "zyxwvu";  s2 = "uoiea";   TRY(s1 >> set_union(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "zyxwvuoiea");
    s1 = "uoiea";   s2 = "zyxwvu";  TRY(s1 >> set_union(s2, std::greater<>()) >> overwrite(s3));  TEST_EQUAL(s3, "zyxwvuoiea");

    s1 = "";        s2 = "";        TRY(s1 << set_union(s2));  TEST_EQUAL(s1, "");
    s1 = "abcdef";  s2 = "";        TRY(s1 << set_union(s2));  TEST_EQUAL(s1, "abcdef");
    s1 = "";        s2 = "abcdef";  TRY(s1 << set_union(s2));  TEST_EQUAL(s1, "abcdef");
    s1 = "abcdef";  s2 = "aeiou";   TRY(s1 << set_union(s2));  TEST_EQUAL(s1, "abcdefiou");
    s1 = "aeiou";   s2 = "abcdef";  TRY(s1 << set_union(s2));  TEST_EQUAL(s1, "abcdefiou");
    s1 = "uvwxyz";  s2 = "aeiou";   TRY(s1 << set_union(s2));  TEST_EQUAL(s1, "aeiouvwxyz");
    s1 = "aeiou";   s2 = "uvwxyz";  TRY(s1 << set_union(s2));  TEST_EQUAL(s1, "aeiouvwxyz");

}

void test_rs_range_combination_zip() {

    std::string s = "abcde", t;
    std::vector<int> iv = {1,2,3,4,5};
    std::vector<std::string> sv;
    std::map<char, int> cim;
    std::map<int, char> icm;
    auto f1 = [] (char c, int i) { return std::string(i, c); };
    auto f2 = [] (int i, char c) { return std::string(i, c); };

    TRY(s >> zip(iv) >> overwrite(cim));                                   TEST_EQUAL(format_map(cim), "{a:1,b:2,c:3,d:4,e:5}");
    TRY(iv >> zip(s) >> overwrite(icm));                                   TEST_EQUAL(format_map(icm), "{1:a,2:b,3:c,4:d,5:e}");
    TRY(s >> passthrough >> zip(iv) >> passthrough >> overwrite(cim));     TEST_EQUAL(format_map(cim), "{a:1,b:2,c:3,d:4,e:5}");
    TRY(iv >> passthrough >> zip(s) >> passthrough >> overwrite(icm));     TEST_EQUAL(format_map(icm), "{1:a,2:b,3:c,4:d,5:e}");
    TRY(s >> zip(iv, f1) >> overwrite(sv));                                TEST_EQUAL(format_range(sv), "[a,bb,ccc,dddd,eeeee]");
    TRY(iv >> zip(s, f2) >> overwrite(sv));                                TEST_EQUAL(format_range(sv), "[a,bb,ccc,dddd,eeeee]");
    TRY(s >> passthrough >> zip(iv, f1) >> passthrough >> overwrite(sv));  TEST_EQUAL(format_range(sv), "[a,bb,ccc,dddd,eeeee]");
    TRY(iv >> passthrough >> zip(s, f2) >> passthrough >> overwrite(sv));  TEST_EQUAL(format_range(sv), "[a,bb,ccc,dddd,eeeee]");
    TRY(s >> passthrough * zip(iv) >> overwrite(cim));                     TEST_EQUAL(format_map(cim), "{a:1,b:2,c:3,d:4,e:5}");
    TRY(s >> zip(iv) * passthrough >> overwrite(cim));                     TEST_EQUAL(format_map(cim), "{a:1,b:2,c:3,d:4,e:5}");
    TRY(s >> passthrough * zip(iv) * passthrough >> overwrite(cim));       TEST_EQUAL(format_map(cim), "{a:1,b:2,c:3,d:4,e:5}");
    TRY(iv >> passthrough * zip(s) >> overwrite(icm));                     TEST_EQUAL(format_map(icm), "{1:a,2:b,3:c,4:d,5:e}");
    TRY(iv >> zip(s) * passthrough >> overwrite(icm));                     TEST_EQUAL(format_map(icm), "{1:a,2:b,3:c,4:d,5:e}");
    TRY(iv >> passthrough * zip(s) * passthrough >> overwrite(icm));       TEST_EQUAL(format_map(icm), "{1:a,2:b,3:c,4:d,5:e}");

}
