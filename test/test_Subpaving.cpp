#include <catch.hpp>

//std
#include <exception>
#include <iostream>
//plsm
#include <plsm/Subpaving.h>
#include <plsm/TestingCommon.h>
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
    s.refine(refine::PolylineDetector<TestType, 3>{0});
}

TEST_CASE("Subpaving 3D", "[Subpaving]")
{
    using Ival = Interval<int>;
    Region<int, 3> r{{Ival{0, 512}, Ival{0, 512}, Ival{0, 512}}};
    Subpaving<int, 3> s(r, {{{2, 2, 2}}});
    std::vector<SpaceVector<int, 3>> rspecPoints;

    SECTION("ball")
    {
        BENCHMARK("refine: ball") {
            s.refine(refine::BallDetector<int, 3>{{256,256,256}, 128});
        };
        // s.render();
    }

    SECTION("z-aligned")
    {
        rspecPoints.push_back({{0, 0, wildcard<int>}});
        rspecPoints.push_back({{256, 128, wildcard<int>}});
        rspecPoints.push_back({{384, 256, wildcard<int>}});
        rspecPoints.push_back({{512, 512, wildcard<int>}});

        BENCHMARK("refine: z-aligned polyline plus box") {
            s.refine(refine::PolylineDetector<int, 3>{rspecPoints});
            Interval<int> ival{0, 56};
            s.refine(refine::RegionDetector<int, 3, Select>{{ival, ival, ival}});
        };
        // s.render();
    }

    SECTION("x-aligned")
    {
        rspecPoints.push_back({{wildcard<int>, 0, 0}});
        rspecPoints.push_back({{wildcard<int>, 256, 128}});
        rspecPoints.push_back({{wildcard<int>, 384, 256}});
        rspecPoints.push_back({{wildcard<int>, 512, 512}});

        BENCHMARK("refine: x-aligned") {
            s.refine(refine::PolylineDetector<int, 3>{rspecPoints});
        };
        // s.render();
    }
}

TEST_CASE("Subpaving 2D(ish)", "[Subpaving]")
{
    using Ival = Interval<int>;
    Region<int, 3> r{{Ival{0, 512}, Ival{0, 512}, Ival{256, 257}}};
    Subpaving<int, 3> s(r, {{{2, 2, 1}}});
    std::vector<SpaceVector<int, 3>> rspecPoints;

    SECTION("ball")
    {
        BENCHMARK("refine: ball 2D-ish") {
            s.refine(refine::BallDetector<int, 3, SelectAll>{{256,256,256}, 128});
        };
        // s.render();
    }

    SECTION("z-aligned")
    {
        rspecPoints.push_back({{0, 0, wildcard<int>}});
        rspecPoints.push_back({{256, 128, wildcard<int>}});
        rspecPoints.push_back({{384, 256, wildcard<int>}});
        rspecPoints.push_back({{512, 512, wildcard<int>}});

        BENCHMARK("refine: z-aligned plus region 2D-ish") {
            s.refine(refine::PolylineDetector<int, 3>{rspecPoints});
            Interval<int> ival{0, 56};
            s.refine(refine::RegionDetector<int, 3, Select>{
                {ival, ival, Interval<int>{0, 512}}});
        };
        // s.render();
    }

    SECTION("x-aligned")
    {
        rspecPoints.push_back({{wildcard<int>, 0, 0}});
        rspecPoints.push_back({{wildcard<int>, 256, 128}});
        rspecPoints.push_back({{wildcard<int>, 384, 256}});
        rspecPoints.push_back({{wildcard<int>, 512, 512}});

        BENCHMARK("refine: x-aligned 2D-ish") {
            s.refine(refine::PolylineDetector<int, 3>{rspecPoints});
        };
        // s.render();
    }
}

TEST_CASE("Subpaving with XRN Defaults", "[Subpaving]")
{
    using Ival = Interval<int>;
    Region<int, 3> r{{Ival{0, 5120}, Ival{0, 4096}, Ival{0, 12}}};
    Subpaving<int, 3> s(r, {{{10, 8, 3}}, {{8, 8, 4}}});
    std::vector<SpaceVector<int, 3>> rspecPoints;
    rspecPoints.push_back({{wildcard<int>, wildcard<int>, 3}});
    // s.refine(refine::PolylineDetector<int, 3>{rspecPoints});
}
