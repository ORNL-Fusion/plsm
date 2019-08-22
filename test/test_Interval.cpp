#define CATCH_CONFIG_MAIN
#include <catch.hpp>

//std
#include <iostream>
#include <sstream>
//plsm
#include <plsm/Interval.h>
#include <plsm/TestingCommon.h>
using namespace plsm;


TEMPLATE_LIST_TEST_CASE("Interval", "[Interval][template]", test::IntTypes)
{
    Interval<TestType> x;
    REQUIRE(x.begin() == 0);
    REQUIRE(x.end() == 0);
    REQUIRE(begin(x) == 0);
    REQUIRE(end(x) == 0);
    REQUIRE(x.empty());
    REQUIRE(x.length() == 0);
    REQUIRE(!x.intersects(x));
    REQUIRE(!x.contains(0));

    Interval<TestType> x1;
    x1 = Interval<TestType>(1);
    REQUIRE(x1.begin() == 0);
    REQUIRE(x1.end() == 1);
    REQUIRE(begin(x1) == 0);
    REQUIRE(end(x1) == 1);
    REQUIRE(!x1.empty());
    REQUIRE(x1.length() == 1);
    REQUIRE(x1.contains(0));
    REQUIRE(!x1.contains(1));
    REQUIRE(!x1.intersects(x));

    std::stringstream ss;
    ss << x1;
    REQUIRE(ss.str() == "[0,1)");

    auto x2 = x1;
    ss.str("");
    ss << x2;
    REQUIRE(ss.str() == "[0,1)");
    REQUIRE(x2 == x1);

    REQUIRE(x2.intersects(x1));

    Interval<TestType> x3{1, 5};
    REQUIRE(x3.begin() == 1);
    REQUIRE(x3.end() == 5);
    REQUIRE(x3.length() == 4);
    REQUIRE(!x3.intersects(x1));
    REQUIRE(!x1.intersects(x3));

    Interval<TestType> x4{4, 7};
    REQUIRE(x4.length() == 3);
    REQUIRE(x4.intersects(x3));
    REQUIRE(x3.intersects(x4));

    REQUIRE(get<RangeElem::first>(x4) == 4);
    REQUIRE(get<RangeElem::last>(x4) == 6);
    REQUIRE(x4.contains(4));
    REQUIRE(x4.contains(6));
    REQUIRE(!x4.contains(7));

    Interval<TestType> x4copy{x4};
    REQUIRE(x4copy.begin() == 4);
    REQUIRE(x4copy.end() == 7);
    REQUIRE(x4copy.length() == 3);

    ss.str("");
    ss << x4;
    REQUIRE(ss.str() == "[4,7)");

    Interval<TestType> x5{15};
    REQUIRE(x != x5);
    x = x5;
    REQUIRE(x == x5);
    REQUIRE(x.begin() == 0);
    REQUIRE(x.end() == 15);
}


TEMPLATE_LIST_TEST_CASE("constexpr", "[Interval][template]", test::IntTypes)
{
    constexpr Interval<TestType> x;
    static_assert(x.begin() == 0, "");
    static_assert(x.end() == 0, "");
    static_assert(begin(x) == 0, "");
    static_assert(end(x) == 0, "");
    static_assert(x.empty(), "");
    static_assert(x.length() == 0, "");

    constexpr Interval<TestType> x1{1};
    static_assert(x1.begin() == 0, "");
    static_assert(x1.end() == 1, "");
    static_assert(begin(x1) == 0, "");
    static_assert(end(x1) == 1, "");
    static_assert(!x1.empty(), "");
    static_assert(x1.length() == 1, "");
    static_assert(x1.contains(0), "");
    static_assert(!x1.contains(1), "");

    {
    std::stringstream ss;
    ss << x1;
    REQUIRE(ss.str() == "[0,1)");
    }

    constexpr auto x2 = x1;
    static_assert(x2.begin() == 0, "");
    static_assert(x2.end() == 1, "");
    static_assert(x2.intersects(x1), "");
    static_assert(x2 == x1, "");

    {
    std::stringstream ss;
    ss << x2;
    REQUIRE(ss.str() == "[0,1)");
    }

    constexpr Interval<TestType> x3{1, 5};
    static_assert(x3.begin() == 1, "");
    static_assert(x3.end() == 5, "");
    static_assert(x3.length() == 4, "");
    static_assert(!x3.intersects(x1), "");
    static_assert(!x1.intersects(x3), "");

    constexpr Interval<TestType> x4{4, 7};
    static_assert(x4.length() == 3, "");
    static_assert(x4.intersects(x3), "");
    static_assert(x3.intersects(x4), "");

    static_assert(get<RangeElem::first>(x4) == 4, "");
    static_assert(get<RangeElem::last>(x4) == 6, "");
    static_assert(x4.contains(4), "");
    static_assert(x4.contains(6), "");
    static_assert(!x4.contains(7), "");

    constexpr Interval<TestType> x4copy{x4};
    static_assert(x4copy.begin() == 4, "");
    static_assert(x4copy.end() == 7, "");
    static_assert(x4copy.length() == 3, "");

    {
    std::stringstream ss;
    ss << x4;
    REQUIRE(ss.str() == "[4,7)");
    }

    constexpr Interval<TestType> x5{15};
    static_assert(x5.begin() == 0, "");
    static_assert(x5.end() == 15, "");
}
