#include <catch.hpp>

//std
//plsm
#include <plsm/detail/EnumIndexed.h>
#include <plsm/Region.h>
#include <plsm/TestingCommon.h>

enum class Axis { x, y, z };

using namespace plsm;
using namespace detail;

TEMPLATE_LIST_TEST_CASE("Enum-Indexed SpaceVector", "[EnumIndexed][template]",
    test::IntTypes)
{
    EnumIndexed<SpaceVector, TestType, 3, Axis> a{5, 4, 3};
    REQUIRE(a[Axis::x] == 5);
    REQUIRE(a[Axis::y] == 4);
    REQUIRE(a[Axis::z] == 3);
}

TEMPLATE_LIST_TEST_CASE("Enum-Indexed Region", "[EnumIndexed][template]",
    test::IntTypes)
{
    using RegionType = EnumIndexed<Region, TestType, 3, Axis>;
    using Ival = typename RegionType::IntervalType;

    RegionType r{{Ival{1, 5}, Ival{3, 9}, Ival{2, 4}}};
    REQUIRE(r[Axis::x] == Ival{1, 5});
    REQUIRE(r[Axis::y] == Ival{3, 9});
    REQUIRE(r[Axis::z] == Ival{2, 4});
}
