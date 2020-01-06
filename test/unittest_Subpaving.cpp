#include <catch.hpp>

#include <exception>
#include <iostream>

#include <plsm/Subpaving.h>
#include <plsm/TestingCommon.h>
#include <plsm/RenderSubpaving.h>
#include <plsm/refine/BallDetector.h>
#include <plsm/refine/PolylineDetector.h>
#include <plsm/refine/RegionDetector.h>
using namespace plsm;

TEMPLATE_LIST_TEST_CASE("Subpaving Basic", "[Subpaving][template]",
    test::IntTypes)
{
    using SubpavingType = Subpaving<TestType, 3>;
    using RegionType = typename SubpavingType::RegionType;
    using Ival = typename RegionType::IntervalType;
    RegionType r{{Ival{0, 4}, Ival{0, 4}, Ival{0, 4}}};
    SubpavingType sp(r, {{{2, 2, 2}}});
    SECTION("Uniform Refinement")
    {
        sp.refine(
            refine::RegionDetector<TestType, 3, Select>{sp.getLatticeRegion()});
        REQUIRE(sp.getTiles(onDevice).extent(0) == 64);
        sp.syncTiles(onHost);
        REQUIRE(sp.getTiles().extent(0) == 64);
        REQUIRE(sp.findTileId({3, 3, 3}) == invalid<std::size_t>);
        sp.syncAll(onHost);
        REQUIRE(sp.findTileId({3, 3, 3}) == 63);
    }
}
