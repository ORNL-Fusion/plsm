#include <catch.hpp>

#include <plsm/TestingCommon.h>
#include <plsm/Tile.h>
using namespace plsm;

TEMPLATE_LIST_TEST_CASE("Tile Basic", "[Tile][template]", test::IntTypes)
{
	using RegionType = Region<TestType, 2>;
	using TileType = Tile<RegionType>;
	TileType t;
	REQUIRE(!t.hasOwningZone());
	REQUIRE(!t.hasData());
	REQUIRE(t.getRegion().empty());

	using Ival = typename RegionType::IntervalType;
	RegionType r1{Ival{12}, Ival{8}};
	Tile<RegionType, TestType> t1{r1, 0};
	REQUIRE(t1.hasOwningZone());
	REQUIRE(t1.getOwningZoneIndex() == 0);
	REQUIRE(!t1.getRegion().empty());
	REQUIRE(!t1.hasData());
	t1.setData(245);
	REQUIRE(t1.hasData());
	REQUIRE(t1.getData() == 245);
}
