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
    using Ival = Interval<TestType>;
    Region<TestType, 3> r{{Ival{0, 4}, Ival{0, 4}, Ival{0, 4}}};
    Subpaving<TestType, 3> s(r, {{{2, 2, 2}}});
    SECTION("Uniform Refinement")
    {
        s.refine(
            refine::RegionDetector<TestType, 3, Select>{s.getLatticeRegion()});
        REQUIRE(s.getNumberOfTiles() == 64);
        REQUIRE(s.getTileId({3, 3, 3}) == 63);
    }
}
