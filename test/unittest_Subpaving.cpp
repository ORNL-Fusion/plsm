#include <catch.hpp>

//std
#include <exception>
#include <iostream>
//plsm
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
    SubpavingType s(r, {{{2, 2, 2}}});
    SECTION("Uniform Refinement")
    {
        s.refine(
            refine::RegionDetector<TestType, 3, Select>{s.getLatticeRegion()});
        REQUIRE(s.getNumberOfTiles() == 64);
        REQUIRE(s.getTileId({3, 3, 3}) == 63);
    }
}
