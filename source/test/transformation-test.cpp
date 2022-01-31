#include "rs-range/transformation.hpp"
#include "rs-format/string.hpp"
#include "rs-tl/iterator.hpp"
#include "rs-unit-test.hpp"
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

using namespace RS::Format;
using namespace RS::Range;
using namespace RS::TL;
using namespace RS::UnitTest;
using namespace std::literals;

void test_rs_range_transformation_pointer_casts() {

    std::string s = "Hello";
    std::vector<char*> cv;
    std::vector<const char*> ccv;

    for (char& c: s) {
        cv.push_back(&c);
        ccv.push_back(&c);
    }

    TRY(ccv >> const_ptr<char> >> each([] (char* p) { if (p) ++*p; }));
    TEST_EQUAL(s, "Ifmmp");

    TRY(cv >> reinterpret_ptr<uint8_t> >> each([] (uint8_t* p) { if (p) --*p; }));
    TEST_EQUAL(s, "Hello");

}

void test_rs_range_transformation_construct() {

    std::vector<std::string> v1;
    std::vector<std::tuple<int, char>> v2 = {
        std::make_tuple(1,'a'),
        std::make_tuple(2,'b'),
        std::make_tuple(3,'c'),
        std::make_tuple(4,'d'),
        std::make_tuple(5,'e')
    };
    std::vector<std::tuple<char, char, char>> v3 = {
        std::make_tuple('a','b','c'),
        std::make_tuple('d','e','f'),
        std::make_tuple('g','h','i'),
        std::make_tuple('j','k','l')
    };

    TRY(v2 >> construct<std::string> >> overwrite(v1));                                 TEST_EQUAL(format_range(v1), "[a,bb,ccc,dddd,eeeee]");
    TRY(v3 >> initialize<std::string> >> overwrite(v1));                                TEST_EQUAL(format_range(v1), "[abc,def,ghi,jkl]");
    TRY(v2 >> passthrough >> construct<std::string> >> passthrough >> overwrite(v1));   TEST_EQUAL(format_range(v1), "[a,bb,ccc,dddd,eeeee]");
    TRY(v3 >> passthrough >> initialize<std::string> >> passthrough >> overwrite(v1));  TEST_EQUAL(format_range(v1), "[abc,def,ghi,jkl]");
    TRY(v2 >> passthrough * construct<std::string> * passthrough >> overwrite(v1));     TEST_EQUAL(format_range(v1), "[a,bb,ccc,dddd,eeeee]");
    TRY(v3 >> passthrough * initialize<std::string> * passthrough >> overwrite(v1));    TEST_EQUAL(format_range(v1), "[abc,def,ghi,jkl]");

}

void test_rs_range_transformation_convert() {

    std::string s;
    std::vector<int> v1, v2 = {71,111,111,100,98,121,101};

    TRY("Hello"s >> convert<int> >> overwrite(v1));                                TEST_EQUAL(format_range(v1), "[72,101,108,108,111]");
    TRY("Hello"s >> passthrough >> convert<int> >> passthrough >> overwrite(v1));  TEST_EQUAL(format_range(v1), "[72,101,108,108,111]");
    TRY("Hello"s >> passthrough * convert<int> * passthrough >> overwrite(v1));    TEST_EQUAL(format_range(v1), "[72,101,108,108,111]");
    TRY(v2 >> convert<char> >> overwrite(s));                                      TEST_EQUAL(s, "Goodbye");
    TRY(v2 >> passthrough >> convert<char> >> passthrough >> overwrite(s));        TEST_EQUAL(s, "Goodbye");
    TRY(v2 >> passthrough * convert<char> * passthrough >> overwrite(s));          TEST_EQUAL(s, "Goodbye");

    v2 = {1,10,100,1000,10000};

    v1 = v2;  TRY(v1 << convert<uint8_t>);                 TEST_EQUAL(format_range(v1), "[1,10,100,232,16]");
    v1 = v2;  TRY(v1 << passthrough << convert<uint8_t>);  TEST_EQUAL(format_range(v1), "[1,10,100,232,16]");

}

void test_rs_range_transformation_dereference() {

    std::vector<std::string> sv;
    std::vector<std::shared_ptr<std::string>> pv = {
        std::make_shared<std::string>("alpha"),
        std::make_shared<std::string>("bravo"),
        std::make_shared<std::string>("charlie")
    };

    TRY(pv >> dereference >> overwrite(sv));                                TEST_EQUAL(format_range(sv), "[alpha,bravo,charlie]");
    TRY(pv >> passthrough >> dereference >> passthrough >> overwrite(sv));  TEST_EQUAL(format_range(sv), "[alpha,bravo,charlie]");
    TRY(pv >> passthrough * dereference * passthrough >> overwrite(sv));    TEST_EQUAL(format_range(sv), "[alpha,bravo,charlie]");

}

void test_rs_range_transformation_indexed() {

    std::vector<std::string> v = {"alpha", "bravo", "charlie", "delta", "echo"};
    std::map<int, std::string> m;

    TRY(v >> indexed >> overwrite(m));                                         TEST_EQUAL(format_map(m), "{0:alpha,1:bravo,2:charlie,3:delta,4:echo}");
    TRY(v >> indexed(1) >> overwrite(m));                                      TEST_EQUAL(format_map(m), "{1:alpha,2:bravo,3:charlie,4:delta,5:echo}");
    TRY(v >> indexed(100, -5) >> overwrite(m));                                TEST_EQUAL(format_map(m), "{80:echo,85:delta,90:charlie,95:bravo,100:alpha}");
    TRY(v >> passthrough >> indexed >> passthrough >> overwrite(m));           TEST_EQUAL(format_map(m), "{0:alpha,1:bravo,2:charlie,3:delta,4:echo}");
    TRY(v >> passthrough >> indexed(1) >> passthrough >> overwrite(m));        TEST_EQUAL(format_map(m), "{1:alpha,2:bravo,3:charlie,4:delta,5:echo}");
    TRY(v >> passthrough >> indexed(100, -5) >> passthrough >> overwrite(m));  TEST_EQUAL(format_map(m), "{80:echo,85:delta,90:charlie,95:bravo,100:alpha}");
    TRY(v >> passthrough * indexed * passthrough >> overwrite(m));             TEST_EQUAL(format_map(m), "{0:alpha,1:bravo,2:charlie,3:delta,4:echo}");
    TRY(v >> passthrough * indexed(1) * passthrough >> overwrite(m));          TEST_EQUAL(format_map(m), "{1:alpha,2:bravo,3:charlie,4:delta,5:echo}");
    TRY(v >> passthrough * indexed(100, -5) * passthrough >> overwrite(m));    TEST_EQUAL(format_map(m), "{80:echo,85:delta,90:charlie,95:bravo,100:alpha}");

}

void test_rs_range_transformation_iterators() {

    std::vector<std::string> v = {"alpha", "bravo", "charlie"};
    std::string s;

    s.clear();  TRY(v >> iterators >> each([&] (auto i) { s += *i + ";"; }));                                TEST_EQUAL(s, "alpha;bravo;charlie;");
    s.clear();  TRY(v >> passthrough >> iterators >> passthrough >> each([&] (auto i) { s += *i + ";"; }));  TEST_EQUAL(s, "alpha;bravo;charlie;");
    s.clear();  TRY(v >> passthrough * iterators * passthrough >> each([&] (auto i) { s += *i + ";"; }));    TEST_EQUAL(s, "alpha;bravo;charlie;");

    TRY(v >> iterators >> each([&] (auto i) { *i = ascii_uppercase(*i); }));
    TEST_EQUAL(format_range(v), "[ALPHA,BRAVO,CHARLIE]");

}

void test_rs_range_transformation_keys_values() {

    std::map<int, std::string> m = {{1, "alpha"}, {2, "bravo"}, {3, "charlie"}};
    std::vector<int> iv;
    std::vector<std::string> sv;

    TRY(m >> keys >> overwrite(iv));                                  TEST_EQUAL(format_range(iv), "[1,2,3]");
    TRY(m >> values >> overwrite(sv));                                TEST_EQUAL(format_range(sv), "[alpha,bravo,charlie]");
    TRY(m >> passthrough >> keys >> passthrough >> overwrite(iv));    TEST_EQUAL(format_range(iv), "[1,2,3]");
    TRY(m >> passthrough >> values >> passthrough >> overwrite(sv));  TEST_EQUAL(format_range(sv), "[alpha,bravo,charlie]");
    TRY(m >> passthrough * keys * passthrough >> overwrite(iv));      TEST_EQUAL(format_range(iv), "[1,2,3]");
    TRY(m >> passthrough * values * passthrough >> overwrite(sv));    TEST_EQUAL(format_range(sv), "[alpha,bravo,charlie]");

}

void test_rs_range_transformation_map() {

    std::string s;
    auto f = [] (char c) { return char(c + 1); };

    TRY("Hello"s >> map(ascii_toupper) >> overwrite(s));                                TEST_EQUAL(s, "HELLO");
    TRY("Hello"s >> map(f) >> overwrite(s));                                            TEST_EQUAL(s, "Ifmmp");
    TRY("Hello"s >> passthrough >> map(ascii_toupper) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "HELLO");
    TRY("Hello"s >> passthrough >> map(f) >> passthrough >> overwrite(s));              TEST_EQUAL(s, "Ifmmp");
    TRY("Hello"s >> passthrough * map(ascii_toupper) * passthrough >> overwrite(s));    TEST_EQUAL(s, "HELLO");
    TRY("Hello"s >> passthrough * map(f) * passthrough >> overwrite(s));                TEST_EQUAL(s, "Ifmmp");

    s = "Hello";  TRY(s << map(ascii_toupper));  TEST_EQUAL(s, "HELLO");

    std::map<int, std::string> m = {{1, "alpha"}, {2, "bravo"}, {3, "charlie"}};
    std::vector<std::string> v;
    auto g = [] (int i, std::string u) { return u + '/' + std::to_string(i); };

    TRY(m >> map_pairs(g) >> overwrite(v));
    TEST_EQUAL(format_range(v), "[alpha/1,bravo/2,charlie/3]");

}

void test_rs_range_transformation_map_if() {

    std::string s;
    auto f = [] (char c) { return char(c + 1); };

    TRY("Hello"s >> map_if(ascii_islower, f) >> overwrite(s));                                TEST_EQUAL(s, "Hfmmp");
    TRY("Hello"s >> passthrough >> map_if(ascii_islower, f) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hfmmp");
    TRY("Hello"s >> passthrough * map_if(ascii_islower, f) * passthrough >> overwrite(s));    TEST_EQUAL(s, "Hfmmp");

    s = "Hello";  TRY(s << map_if(ascii_islower, f));  TEST_EQUAL(s, "Hfmmp");

}

void test_rs_range_transformation_replace() {

    std::string s;

    TRY("Hello World"s >> replace('l', '*') >> overwrite(s));                                             TEST_EQUAL(s, "He**o Wor*d");
    TRY("Hello World"s >> replace('x', '*') >> overwrite(s));                                             TEST_EQUAL(s, "Hello World");
    TRY("Hello World"s >> replace_if(ascii_islower, '*') >> overwrite(s));                                TEST_EQUAL(s, "H**** W****");
    TRY("Hello World"s >> replace_if(ascii_ispunct, '*') >> overwrite(s));                                TEST_EQUAL(s, "Hello World");
    TRY("Hello World"s >> passthrough >> replace('l', '*') >> passthrough >> overwrite(s));               TEST_EQUAL(s, "He**o Wor*d");
    TRY("Hello World"s >> passthrough >> replace('x', '*') >> passthrough >> overwrite(s));               TEST_EQUAL(s, "Hello World");
    TRY("Hello World"s >> passthrough >> replace_if(ascii_islower, '*') >> passthrough >> overwrite(s));  TEST_EQUAL(s, "H**** W****");
    TRY("Hello World"s >> passthrough >> replace_if(ascii_ispunct, '*') >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello World");
    TRY("Hello World"s >> passthrough * replace('l', '*') * passthrough >> overwrite(s));                 TEST_EQUAL(s, "He**o Wor*d");
    TRY("Hello World"s >> passthrough * replace('x', '*') * passthrough >> overwrite(s));                 TEST_EQUAL(s, "Hello World");
    TRY("Hello World"s >> passthrough * replace_if(ascii_islower, '*') * passthrough >> overwrite(s));    TEST_EQUAL(s, "H**** W****");
    TRY("Hello World"s >> passthrough * replace_if(ascii_ispunct, '*') * passthrough >> overwrite(s));    TEST_EQUAL(s, "Hello World");

    s = "Hello World";  TRY(s << replace('l', '*'));               TEST_EQUAL(s, "He**o Wor*d");
    s = "Hello World";  TRY(s << replace('x', '*'));               TEST_EQUAL(s, "Hello World");
    s = "Hello World";  TRY(s << replace_if(ascii_islower, '*'));  TEST_EQUAL(s, "H**** W****");
    s = "Hello World";  TRY(s << replace_if(ascii_ispunct, '*'));  TEST_EQUAL(s, "Hello World");

}

void test_rs_range_transformation_swap_pairs() {

    std::map<int, std::string> ism = {{1, "alpha"}, {2, "bravo"}, {3, "charlie"}};
    std::map<std::string, int> sim;

    TRY(ism >> swap_pairs >> overwrite(sim));                                TEST_EQUAL(format_map(sim), "{alpha:1,bravo:2,charlie:3}");
    TRY(ism >> passthrough >> swap_pairs >> passthrough >> overwrite(sim));  TEST_EQUAL(format_map(sim), "{alpha:1,bravo:2,charlie:3}");
    TRY(ism >> passthrough * swap_pairs * passthrough >> overwrite(sim));    TEST_EQUAL(format_map(sim), "{alpha:1,bravo:2,charlie:3}");

    std::map<int, int> iim;
    for (int i = 1; i <= 10; ++i)
        iim[i] = - i * i;
    TEST_EQUAL(format_map(iim), "{1:-1,2:-4,3:-9,4:-16,5:-25,6:-36,7:-49,8:-64,9:-81,10:-100}");
    TRY(iim << swap_pairs);
    TEST_EQUAL(format_map(iim), "{-100:10,-81:9,-64:8,-49:7,-36:6,-25:5,-16:4,-9:3,-4:2,-1:1}");

}
