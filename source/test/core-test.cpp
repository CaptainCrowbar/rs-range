#include "rs-range/core.hpp"
#include "rs-format/string.hpp"
#include "rs-tl/iterator.hpp"
#include "rs-unit-test.hpp"
#include <algorithm>
#include <forward_list>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace RS::Format;
using namespace RS::Range;
using namespace RS::TL;
using namespace RS::UnitTest;
using namespace std::literals;

void test_rs_range_core_infrastructure() {

    using namespace RS::Range::Detail;

    using Itag = std::input_iterator_tag;
    using Otag = std::output_iterator_tag;
    using Ftag = std::forward_iterator_tag;
    using Btag = std::bidirectional_iterator_tag;
    using Rtag = std::random_access_iterator_tag;

    { using T = RangeIterator<std::string>;         TEST_TYPE(T, std::string::iterator); }
    { using T = RangeIterator<std::string&>;        TEST_TYPE(T, std::string::iterator); }
    { using T = RangeIterator<const std::string>;   TEST_TYPE(T, std::string::const_iterator); }
    { using T = RangeIterator<const std::string&>;  TEST_TYPE(T, std::string::const_iterator); }
    { using T = RangeValue<std::string>;            TEST_TYPE(T, char); }
    { using T = RangeValue<std::string&>;           TEST_TYPE(T, char); }
    { using T = RangeValue<const std::string>;      TEST_TYPE(T, char); }
    { using T = RangeValue<const std::string&>;     TEST_TYPE(T, char); }

    TEST((! is_range<void>));
    TEST((! is_range<int>));
    TEST((is_range<std::string>));
    TEST((is_range<std::string&>));
    TEST((is_range<const std::string>));
    TEST((is_range<const std::string&>));
    TEST((is_range<std::map<int, std::string>>));
    TEST((! is_range<std::unique_ptr<std::string>>));

    TEST((! is_iterator<void>));
    TEST((! is_iterator<int>));
    TEST((is_iterator<int*>));
    TEST((! is_iterator<std::string>));
    TEST((is_iterator<std::string::iterator>));
    TEST((is_iterator<std::string::const_iterator>));
    TEST((! is_iterator<std::map<int, std::string>>));
    TEST((is_iterator<std::map<int, std::string>::iterator>));
    TEST((is_iterator<std::map<int, std::string>::const_iterator>));
    TEST((! is_iterator<std::unique_ptr<std::string>>));

    { using T = IteratorCategory<Itag>;                         TEST_TYPE(T, Itag); }
    { using T = IteratorCategory<Otag>;                         TEST_TYPE(T, Otag); }
    { using T = IteratorCategory<Ftag>;                         TEST_TYPE(T, Ftag); }
    { using T = IteratorCategory<Btag>;                         TEST_TYPE(T, Btag); }
    { using T = IteratorCategory<Rtag>;                         TEST_TYPE(T, Rtag); }
    { using T = IteratorCategory<std::forward_list<int>>;       TEST_TYPE(T, Ftag); }
    { using T = IteratorCategory<std::vector<int>>;             TEST_TYPE(T, Rtag); }
    { using T = IteratorCategory<std::string>;                  TEST_TYPE(T, Rtag); }
    { using T = IteratorCategory<std::string::iterator>;        TEST_TYPE(T, Rtag); }
    { using T = IteratorCategory<std::string::const_iterator>;  TEST_TYPE(T, Rtag); }
    { using T = IteratorCategory<int*>;                         TEST_TYPE(T, Rtag); }

    { using T = CommonCategory<Itag, Itag>;                                TEST_TYPE(T, Itag); }
    { using T = CommonCategory<Itag, Ftag>;                                TEST_TYPE(T, Itag); }
    { using T = CommonCategory<Itag, Btag>;                                TEST_TYPE(T, Itag); }
    { using T = CommonCategory<Itag, Rtag>;                                TEST_TYPE(T, Itag); }
    { using T = CommonCategory<Ftag, Itag>;                                TEST_TYPE(T, Itag); }
    { using T = CommonCategory<Ftag, Ftag>;                                TEST_TYPE(T, Ftag); }
    { using T = CommonCategory<Ftag, Btag>;                                TEST_TYPE(T, Ftag); }
    { using T = CommonCategory<Ftag, Rtag>;                                TEST_TYPE(T, Ftag); }
    { using T = CommonCategory<Btag, Itag>;                                TEST_TYPE(T, Itag); }
    { using T = CommonCategory<Btag, Ftag>;                                TEST_TYPE(T, Ftag); }
    { using T = CommonCategory<Btag, Btag>;                                TEST_TYPE(T, Btag); }
    { using T = CommonCategory<Btag, Rtag>;                                TEST_TYPE(T, Btag); }
    { using T = CommonCategory<Rtag, Itag>;                                TEST_TYPE(T, Itag); }
    { using T = CommonCategory<Rtag, Ftag>;                                TEST_TYPE(T, Ftag); }
    { using T = CommonCategory<Rtag, Btag>;                                TEST_TYPE(T, Btag); }
    { using T = CommonCategory<Rtag, Rtag>;                                TEST_TYPE(T, Rtag); }
    { using T = CommonCategory<Ftag, Btag, Rtag>;                          TEST_TYPE(T, Ftag); }
    { using T = CommonCategory<Rtag, Btag, Ftag>;                          TEST_TYPE(T, Ftag); }
    { using T = CommonCategory<Itag, Ftag, Btag, Rtag>;                    TEST_TYPE(T, Itag); }
    { using T = CommonCategory<Rtag, Btag, Ftag, Itag>;                    TEST_TYPE(T, Itag); }
    { using T = CommonCategory<std::vector<int>, std::forward_list<int>>;  TEST_TYPE(T, Ftag); }
    { using T = CommonCategory<std::forward_list<int>, std::vector<int>>;  TEST_TYPE(T, Ftag); }

    TEST((! category_equals<std::forward_list<int>, Itag>));
    TEST((category_equals<std::forward_list<int>, Ftag>));
    TEST((! category_equals<std::forward_list<int>, Btag>));
    TEST((! category_equals<std::forward_list<int>, Rtag>));
    TEST((! category_equals<std::string, Itag>));
    TEST((! category_equals<std::string, Ftag>));
    TEST((! category_equals<std::string, Btag>));
    TEST((category_equals<std::string, Rtag>));
    TEST((! category_equals<int*, Itag>));
    TEST((! category_equals<int*, Ftag>));
    TEST((! category_equals<int*, Btag>));
    TEST((category_equals<int*, Rtag>));

    TEST((category_is_at_least<std::forward_list<int>, Itag>));
    TEST((category_is_at_least<std::forward_list<int>, Ftag>));
    TEST((! category_is_at_least<std::forward_list<int>, Btag>));
    TEST((! category_is_at_least<std::forward_list<int>, Rtag>));
    TEST((category_is_at_least<std::string, Itag>));
    TEST((category_is_at_least<std::string, Ftag>));
    TEST((category_is_at_least<std::string, Btag>));
    TEST((category_is_at_least<std::string, Rtag>));
    TEST((category_is_at_least<int*, Itag>));
    TEST((category_is_at_least<int*, Ftag>));
    TEST((category_is_at_least<int*, Btag>));
    TEST((category_is_at_least<int*, Rtag>));

}

void test_rs_range_core_output() {

    std::string s;

    s.clear();  TRY("Hello"s >> output(std::back_inserter(s)));                                                              TEST_EQUAL(s, "Hello");
    s.clear();  TRY("Hello"s >> passthrough >> output(std::back_inserter(s)));                                               TEST_EQUAL(s, "Hello");
    s.clear();  TRY("Hello"s >> passthrough >> passthrough >> output(std::back_inserter(s)));                                TEST_EQUAL(s, "Hello");
    s.clear();  TRY("Hello"s >> passthrough >> passthrough >> passthrough >> output(std::back_inserter(s)));                 TEST_EQUAL(s, "Hello");
    s.clear();  TRY("Hello"s >> passthrough >> passthrough >> passthrough >> passthrough >> output(std::back_inserter(s)));  TEST_EQUAL(s, "Hello");

}

void test_rs_range_core_passthrough() {

    std::string s;

    TRY("Hello"s >> overwrite(s));                                                              TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough >> overwrite(s));                                               TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough >> passthrough >> overwrite(s));                                TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough >> passthrough >> passthrough >> overwrite(s));                 TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough >> passthrough >> passthrough >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello");

}

void test_rs_range_core_collect() {

    std::string s;
    std::forward_list<char> fl = {'H','e','l','l','o'};

    auto r1 = fl >> collect;
    TRY(std::reverse(r1.begin(), r1.end()));
    TRY(r1 >> overwrite(s));
    TEST_EQUAL(s, "olleH");

    auto r2 = fl >> passthrough >> collect;
    TRY(std::reverse(r2.begin(), r2.end()));
    TRY(r2 >> passthrough >> overwrite(s));
    TEST_EQUAL(s, "olleH");

    TRY(s = fl >> collect_as<std::string>);
    TEST_EQUAL(s, "Hello");

    TRY(s = fl >> passthrough >> collect_as<std::string>);
    TEST_EQUAL(s, "Hello");

    std::optional<char> oc;
    TRY(oc >> overwrite(s));
    TEST_EQUAL(s, "");
    oc = 'A';
    TRY(oc >> overwrite(s));
    TEST_EQUAL(s, "A");

}

void test_rs_range_core_each() {

    std::string s;

    auto f1 = [&s] (char c) { s += c; s += '.'; };
    TRY("Hello"s >> each(f1));
    TEST_EQUAL(s, "H.e.l.l.o.");
    s.clear();
    TRY("Hello"s >> passthrough >> each(f1));
    TEST_EQUAL(s, "H.e.l.l.o.");

    auto f2 = [] (char& c) { c = ascii_toupper(c); };
    s = "Hello";
    TRY(s << each(f2));
    TEST_EQUAL(s, "HELLO");

    std::map<int, std::string> m = {{1, "alpha"}, {2, "bravo"}, {3, "charlie"}};

    auto f3 = [&s] (int k, std::string v) { s += std::to_string(k) + ':' + v + ';'; };
    s.clear();
    TRY(m >> each_pair(f3));
    TEST_EQUAL(s, "1:alpha;2:bravo;3:charlie;");
    s.clear();
    TRY(m >> passthrough >> each_pair(f3));
    TEST_EQUAL(s, "1:alpha;2:bravo;3:charlie;");

    auto f4 = [] (int k, std::string& v) { v += '.' + std::to_string(k); };
    TRY(m << each_pair(f4));
    TRY(s = format_map(m));
    TEST_EQUAL(s, "{1:alpha.1,2:bravo.2,3:charlie.3}");

    std::optional<char> oc;
    s.clear();
    TRY(oc >> each(f1));
    TEST_EQUAL(s, "");
    oc = 'a';
    s.clear();
    TRY(oc >> each(f1));
    TEST_EQUAL(s, "a.");

    oc = {};
    TRY(oc << each(f2));
    TEST(! oc.has_value());
    oc = 'a';
    TRY(oc << each(f2));
    TEST(oc.has_value());
    TEST_EQUAL(oc.value(), 'A');

}

void test_rs_range_core_combinator() {

    std::string s;

    auto c1 = passthrough * passthrough;
    TRY("Hello"s >> c1 >> overwrite(s));
    TEST_EQUAL(s, "Hello");

    auto f1 = [] (char& c) { c = ascii_toupper(c); };
    auto f2 = [] (char& c) { c += 0x20; };
    auto c2 = each(f1) * each(f2);
    s = "Hello";
    TRY(s << c2);
    TEST_EQUAL(s, "hello");

}
