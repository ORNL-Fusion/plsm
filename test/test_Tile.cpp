#define CATCH_CONFIG_MAIN
#include <catch.hpp>

//plsm
#include <plsm/Tile.h>
#include <plsm/TestingCommon.h>
using namespace plsm;

TEMPLATE_LIST_TEST_CASE("Basic", "[Tile][template]", test::IntTypes)
{
    using TileType = Tile<TestType, 2>;
    TileType t;
    REQUIRE(!t.hasOwningZone());
    REQUIRE(!t.hasData());
    REQUIRE(t.getRegion().empty());

    using RegionType = typename TileType::RegionType;
    using Ival = typename RegionType::IntervalType;
    RegionType r1{Ival{12}, Ival{8}};
    TileType t1{r1, 0};
    REQUIRE(t1.hasOwningZone());
    REQUIRE(t1.getOwningZoneIndex() == 0);
    REQUIRE(!t1.getRegion().empty());
    REQUIRE(!t1.hasData());
    t1.setData(245);
    REQUIRE(t1.hasData());
    REQUIRE(t1.getData() == 245);
}
