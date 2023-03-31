#include <catch.hpp>

#include <plsm/EnumIndexed.h>
#include <plsm/MultiIndex.h>
#include <plsm/Region.h>
#include <plsm/TestingCommon.h>

using namespace plsm;
using test::Axis;

TEMPLATE_LIST_TEST_CASE(
	"Enum-Indexed SpaceVector", "[EnumIndexed][template]", test::IntTypes)
{
	using Vector = EnumIndexed<SpaceVector<TestType, 3>, Axis>;
	Vector a{5, 4, 3};
	REQUIRE(a[Axis::x] == 5);
	REQUIRE(a[Axis::y] == 4);
	REQUIRE(a[Axis::z] == 3);
}

TEMPLATE_LIST_TEST_CASE(
	"Enum-Indexed Region", "[EnumIndexed][template]", test::IntTypes)
{
	using RegionType = EnumIndexed<Region<TestType, 3>, Axis>;
	using Ival = typename RegionType::IntervalType;

	RegionType r{{Ival{1, 5}, Ival{3, 9}, Ival{2, 4}}};
	REQUIRE(r[Axis::x] == Ival{1, 5});
	REQUIRE(r[Axis::y] == Ival{3, 9});
	REQUIRE(r[Axis::z] == Ival{2, 4});
}

TEST_CASE("Enum-Indexed MultiIndex")
{
	EnumIndexed<MultiIndex<3>, Axis> i3{2, 3, 4};
	REQUIRE(i3[Axis::x] == 2);
	REQUIRE(i3[Axis::y] == 3);
	REQUIRE(i3[Axis::z] == 4);
}

TEST_CASE("Void Index Type")
{
	static_assert(std::is_same<EnumIndexed<MultiIndex<3>>, MultiIndex<3>>{});
}
