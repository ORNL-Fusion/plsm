#include <catch.hpp>

//std
#include <vector>
//plsm
#include <plsm/IntervalRange.h>
#include <plsm/TestingCommon.h>
using namespace plsm;

TEMPLATE_LIST_TEST_CASE("IntervalRange Basic", "[IntervalRange][template]",
    test::IntTypes)
{
    IntervalRange<TestType> ir0;
    IntervalRange<TestType> ir1{16};
    IntervalRange<TestType> ir2{12, 20};
    Interval<TestType> ival{12, 20};
    IntervalRange<TestType> ir3{ival};

    SECTION("Constructed State")
    {
        REQUIRE(ir0.interval() == Interval<TestType>{});
        REQUIRE(ir1.interval() == Interval<TestType>{16});
        REQUIRE(ir2.interval() == ival);
        REQUIRE(ir3.interval() == ival);
        REQUIRE(makeIntervalRange(ir1.interval().end()) == ir1);
        REQUIRE(makeIntervalRange(ival.begin(), ival.end()) == ir2);
        REQUIRE(makeIntervalRange(ival) == ir3);
    }

    SECTION("begin/end")
    {
        REQUIRE(*ir0.begin() == 0);
        REQUIRE(*ir0.end() == 0);
        REQUIRE(std::distance(ir0.begin(), ir0.end()) == 0);
        REQUIRE(*ir1.begin() == 0);
        REQUIRE(*ir1.end() == 16);
        REQUIRE(std::distance(ir1.begin(), ir1.end()) == 16);
        REQUIRE(*ir2.begin() == 12);
        REQUIRE(*ir2.end() == 20);
        REQUIRE(std::distance(ir2.begin(), ir2.end()) == 8);
        REQUIRE(*ir2.begin() == ival.begin());
        REQUIRE(*ir2.end() == ival.end());
    }
}

TEMPLATE_LIST_TEST_CASE("IntervalRange Range-for", "[IntervalRange][template]",
    test::IntTypes)
{
    std::vector<TestType> v;

    SECTION("0..10")
    {
        for (auto elem : makeIntervalRange(static_cast<TestType>(10))) {
            v.push_back(elem);
        }
        REQUIRE(v == std::vector<TestType>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    }

    SECTION("5..10")
    {
        for (auto elem : makeIntervalRange<TestType>(5, 10)) {
            v.push_back(elem);
        }
        REQUIRE(v == std::vector<TestType>{5, 6, 7, 8, 9});
    }

    SECTION("ival")
    {
        Interval<TestType> ival{10, 15};
        for (auto elem : makeIntervalRange(ival)) {
            v.push_back(elem);
        }
        REQUIRE(v == std::vector<TestType>{10, 11, 12, 13, 14});
    }
}

TEMPLATE_LIST_TEST_CASE("IntervalRange constexpr", "[IntervalRange][template]",
    test::IntTypes)
{
    constexpr IntervalRange<TestType> ir0;
    constexpr IntervalRange<TestType> ir1{16};
    constexpr IntervalRange<TestType> ir2{12, 20};
    constexpr Interval<TestType> ival{12, 20};
    constexpr IntervalRange<TestType> ir3{ival};

    SECTION("Constructed State")
    {
        static_assert(ir0.interval() == Interval<TestType>{}, "");
        static_assert(ir1.interval() == Interval<TestType>{16}, "");
        static_assert(ir2.interval() == ival, "");
        static_assert(ir3.interval() == ival, "");
        static_assert(makeIntervalRange(ir1.interval().end()) == ir1, "");
        static_assert(makeIntervalRange(ival.begin(), ival.end()) == ir2, "");
        static_assert(makeIntervalRange(ival) == ir3, "");
    }

    SECTION("begin/end")
    {
        static_assert(*ir0.begin() == 0, "");
        static_assert(*ir0.end() == 0, "");
        static_assert(*ir1.begin() == 0, "");
        static_assert(*ir1.end() == 16, "");
        static_assert(*ir2.begin() == 12, "");
        static_assert(*ir2.end() == 20, "");
        static_assert(*ir2.begin() == ival.begin(), "");
        static_assert(*ir2.end() == ival.end(), "");
    }
}
