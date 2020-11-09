#include <catch.hpp>

#include <plsm/TestingCommon.h>
#include <plsm/Zone.h>
using namespace plsm;

TEMPLATE_LIST_TEST_CASE("Zone Basic", "[Zone][template]", test::IntTypes)
{
	using RegionType = Region<TestType, 2>;
	using ZoneType = Zone<RegionType>;
	ZoneType zone;
	REQUIRE(!zone.hasTile());
	REQUIRE(!zone.hasParent());
	REQUIRE(zone.getRegion().empty());
	REQUIRE(zone.getSubZoneIndices().empty());

	using Ival = typename RegionType::IntervalType;
	RegionType r1{Ival{12}, Ival{8}};
	ZoneType zone1{r1, 0};
	REQUIRE(!zone1.hasTile());
	REQUIRE(!zone1.getRegion().empty());
	REQUIRE(!zone1.hasParent());

	ZoneType zone2{r1, 1, 0};
	REQUIRE(!zone2.hasTile());
	REQUIRE(!zone2.getRegion().empty());
	REQUIRE(zone2.getSubZoneIndices().empty());
	REQUIRE(zone2.hasParent());
	REQUIRE(zone2.getParentIndex() == 0);
	zone2.setTileIndex(1);
	REQUIRE(zone2.hasTile());
	REQUIRE(zone2.getTileIndex() == 1);

	Interval<std::size_t> ival{8};
	zone2.setSubZoneIndices(ival);
	REQUIRE(zone2.getSubZoneIndices() == ival);
	REQUIRE(zone2.getSubZoneRange().interval() == ival);
}
