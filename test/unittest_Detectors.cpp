#include <catch.hpp>

#include <type_traits>

#include <plsm/Region.h>
#include <plsm/refine/BallDetector.h>
#include <plsm/refine/PolylineDetector.h>
#include <plsm/refine/RegionDetector.h>
#include <plsm/TestingCommon.h>
using namespace plsm;

TEMPLATE_LIST_TEST_CASE("PolylineDetector - 3D", "[Detectors][template]",
    test::IntTypes)
{
    // using Ival = Interval<TestType>;
    // refine::PolylineDetector<TestType, 3> pd3;
    // TODO
}

TEMPLATE_LIST_TEST_CASE("RegionDetector - 2D", "[Detectors][template]",
    test::IntTypes)
{
    using Ival = Interval<TestType>;
    Region<TestType, 2> r{{Ival{128}, Ival{128}}};

    SECTION("Intersect Mode")
    {
        using DetectorType = refine::RegionDetector<TestType, 2>;
        // using RefineTag = typename DetectorType::RefineTag;
        // using SelectTag = typename DetectorType::SelectTag;
        DetectorType rd2{{Ival{32, 96}, Ival{32, 96}}};

        REQUIRE(rd2.intersect(r));
    }

    //TODO: Need to distinguish between intersect and overlap like BallDetector
}

TEMPLATE_LIST_TEST_CASE("RegionDetector - 3D", "[Detectors][template]",
    test::IntTypes)
{
    using Ival = Interval<TestType>;
    refine::RegionDetector<TestType, 3> rd3{{Ival{56}, Ival{56}, Ival{56}}};
}

TEMPLATE_LIST_TEST_CASE("BallDetector - 2D", "[Detectors][template]",
    test::IntTypes)
{
    using Ival = Interval<TestType>;
    Region<TestType, 2> r{{Ival{0, 128}, Ival{0, 128}}};
    Region<TestType, 2> r1{{Ival{0, 64}, Ival{0, 64}}};
    Region<TestType, 2> r2{{Ival{0, 64}, Ival{64, 128}}};
    Region<TestType, 2> r3{{Ival{64, 128}, Ival{0, 64}}};
    Region<TestType, 2> r4{{Ival{64, 128}, Ival{64, 128}}};
    Region<TestType, 2> r5{{Ival{24, 32}, Ival{32, 96}}};
    Region<TestType, 2> r6{{Ival{32, 32}, Ival{32, 32}}};
    Region<TestType, 2> r7{{Ival{16}, Ival{16}}};
    Region<TestType, 2> r8{{Ival{16}, Ival{112, 128}}};
    Region<TestType, 2> r9{{Ival{112, 128}, Ival{16}}};
    Region<TestType, 2> r10{{Ival{112, 128}, Ival{112, 128}}};

    SECTION("Intersect Mode")
    {
        using DetectorType = refine::BallDetector<TestType, 2>;
        using RefineTag = typename DetectorType::RefineTag;
        using SelectTag = typename DetectorType::SelectTag;
        DetectorType bd2{{64, 64}, 64};
        REQUIRE(bd2.intersect(r));
        REQUIRE(bd2.intersect(r1));
        REQUIRE(bd2.intersect(r2));
        REQUIRE(bd2.intersect(r3));
        REQUIRE(bd2.intersect(r4));
        REQUIRE(!bd2.intersect(r5));
        REQUIRE(!bd2.intersect(r6));
        REQUIRE(!bd2.intersect(r7));
        REQUIRE(!bd2.intersect(r8));
        REQUIRE(!bd2.intersect(r9));
        REQUIRE(!bd2.intersect(r10));
        REQUIRE(bd2(Intersect{}, r));
        REQUIRE(bd2(Intersect{}, r1));
        REQUIRE(bd2(Intersect{}, r2));
        REQUIRE(bd2(Intersect{}, r3));
        REQUIRE(bd2(Intersect{}, r4));
        REQUIRE(!bd2(Intersect{}, r5));
        REQUIRE(!bd2(Intersect{}, r6));
        REQUIRE(!bd2(Intersect{}, r7));
        REQUIRE(!bd2(Intersect{}, r8));
        REQUIRE(!bd2(Intersect{}, r9));
        REQUIRE(!bd2(Intersect{}, r10));
        REQUIRE(bd2(RefineTag{}, r));
        REQUIRE(bd2(RefineTag{}, r1));
        REQUIRE(bd2(RefineTag{}, r2));
        REQUIRE(bd2(RefineTag{}, r3));
        REQUIRE(bd2(RefineTag{}, r4));
        REQUIRE(!bd2(RefineTag{}, r5));
        REQUIRE(!bd2(RefineTag{}, r6));
        REQUIRE(!bd2(RefineTag{}, r7));
        REQUIRE(!bd2(RefineTag{}, r8));
        REQUIRE(!bd2(RefineTag{}, r9));
        REQUIRE(!bd2(RefineTag{}, r10));
        REQUIRE(bd2.overlap(r));
        REQUIRE(bd2.overlap(r1));
        REQUIRE(bd2.overlap(r2));
        REQUIRE(bd2.overlap(r3));
        REQUIRE(bd2.overlap(r4));
        REQUIRE(bd2.overlap(r5));
        REQUIRE(bd2.overlap(r6));
        REQUIRE(!bd2.overlap(r7));
        REQUIRE(!bd2.overlap(r8));
        REQUIRE(!bd2.overlap(r9));
        REQUIRE(!bd2.overlap(r10));
        REQUIRE(bd2.select(r));
        REQUIRE(bd2.select(r1));
        REQUIRE(bd2.select(r2));
        REQUIRE(bd2.select(r3));
        REQUIRE(bd2.select(r4));
        REQUIRE(bd2.select(r5));
        REQUIRE(bd2.select(r6));
        REQUIRE(!bd2.select(r7));
        REQUIRE(!bd2.select(r8));
        REQUIRE(!bd2.select(r9));
        REQUIRE(!bd2.select(r10));
        REQUIRE(bd2(Select{}, r));
        REQUIRE(bd2(Select{}, r1));
        REQUIRE(bd2(Select{}, r2));
        REQUIRE(bd2(Select{}, r3));
        REQUIRE(bd2(Select{}, r4));
        REQUIRE(bd2(Select{}, r5));
        REQUIRE(bd2(Select{}, r6));
        REQUIRE(!bd2(Select{}, r7));
        REQUIRE(!bd2(Select{}, r8));
        REQUIRE(!bd2(Select{}, r9));
        REQUIRE(!bd2(Select{}, r10));
        REQUIRE(bd2(SelectTag{}, r));
        REQUIRE(bd2(SelectTag{}, r1));
        REQUIRE(bd2(SelectTag{}, r2));
        REQUIRE(bd2(SelectTag{}, r3));
        REQUIRE(bd2(SelectTag{}, r4));
        REQUIRE(bd2(SelectTag{}, r5));
        REQUIRE(bd2(SelectTag{}, r6));
        REQUIRE(!bd2(SelectTag{}, r7));
        REQUIRE(!bd2(SelectTag{}, r8));
        REQUIRE(!bd2(SelectTag{}, r9));
        REQUIRE(!bd2(SelectTag{}, r10));
    }

    SECTION("Select Mode")
    {
        using DetectorType = refine::BallDetector<TestType, 2, Select>;
        using RefineTag = typename DetectorType::RefineTag;
        using SelectTag = typename DetectorType::SelectTag;
        DetectorType bd2{{64, 64}, 64};
        REQUIRE(bd2.intersect(r));
        REQUIRE(bd2.intersect(r1));
        REQUIRE(bd2.intersect(r2));
        REQUIRE(bd2.intersect(r3));
        REQUIRE(bd2.intersect(r4));
        REQUIRE(!bd2.intersect(r5));
        REQUIRE(!bd2.intersect(r6));
        REQUIRE(!bd2.intersect(r7));
        REQUIRE(!bd2.intersect(r8));
        REQUIRE(!bd2.intersect(r9));
        REQUIRE(!bd2.intersect(r10));
        REQUIRE(bd2(Intersect{}, r));
        REQUIRE(bd2(Intersect{}, r1));
        REQUIRE(bd2(Intersect{}, r2));
        REQUIRE(bd2(Intersect{}, r3));
        REQUIRE(bd2(Intersect{}, r4));
        REQUIRE(!bd2(Intersect{}, r5));
        REQUIRE(!bd2(Intersect{}, r6));
        REQUIRE(!bd2(Intersect{}, r7));
        REQUIRE(!bd2(Intersect{}, r8));
        REQUIRE(!bd2(Intersect{}, r9));
        REQUIRE(!bd2(Intersect{}, r10));
        REQUIRE(bd2(RefineTag{}, r));
        REQUIRE(bd2(RefineTag{}, r1));
        REQUIRE(bd2(RefineTag{}, r2));
        REQUIRE(bd2(RefineTag{}, r3));
        REQUIRE(bd2(RefineTag{}, r4));
        REQUIRE(bd2(RefineTag{}, r5));
        REQUIRE(bd2(RefineTag{}, r6));
        REQUIRE(!bd2(RefineTag{}, r7));
        REQUIRE(!bd2(RefineTag{}, r8));
        REQUIRE(!bd2(RefineTag{}, r9));
        REQUIRE(!bd2(RefineTag{}, r10));
        REQUIRE(bd2.overlap(r));
        REQUIRE(bd2.overlap(r1));
        REQUIRE(bd2.overlap(r2));
        REQUIRE(bd2.overlap(r3));
        REQUIRE(bd2.overlap(r4));
        REQUIRE(bd2.overlap(r5));
        REQUIRE(bd2.overlap(r6));
        REQUIRE(!bd2.overlap(r7));
        REQUIRE(!bd2.overlap(r8));
        REQUIRE(!bd2.overlap(r9));
        REQUIRE(!bd2.overlap(r10));
        REQUIRE(bd2.select(r));
        REQUIRE(bd2.select(r1));
        REQUIRE(bd2.select(r2));
        REQUIRE(bd2.select(r3));
        REQUIRE(bd2.select(r4));
        REQUIRE(bd2.select(r5));
        REQUIRE(bd2.select(r6));
        REQUIRE(!bd2.select(r7));
        REQUIRE(!bd2.select(r8));
        REQUIRE(!bd2.select(r9));
        REQUIRE(!bd2.select(r10));
        REQUIRE(bd2(Select{}, r));
        REQUIRE(bd2(Select{}, r1));
        REQUIRE(bd2(Select{}, r2));
        REQUIRE(bd2(Select{}, r3));
        REQUIRE(bd2(Select{}, r4));
        REQUIRE(bd2(Select{}, r5));
        REQUIRE(bd2(Select{}, r6));
        REQUIRE(!bd2(Select{}, r7));
        REQUIRE(!bd2(Select{}, r8));
        REQUIRE(!bd2(Select{}, r9));
        REQUIRE(!bd2(Select{}, r10));
        REQUIRE(bd2(SelectTag{}, r));
        REQUIRE(bd2(SelectTag{}, r1));
        REQUIRE(bd2(SelectTag{}, r2));
        REQUIRE(bd2(SelectTag{}, r3));
        REQUIRE(bd2(SelectTag{}, r4));
        REQUIRE(bd2(SelectTag{}, r5));
        REQUIRE(bd2(SelectTag{}, r6));
        REQUIRE(bd2(SelectTag{}, r7));
        REQUIRE(bd2(SelectTag{}, r8));
        REQUIRE(bd2(SelectTag{}, r9));
        REQUIRE(bd2(SelectTag{}, r10));
    }

    SECTION("SelectAll Mode")
    {
        using DetectorType = refine::BallDetector<TestType, 2, SelectAll>;
        using RefineTag = typename DetectorType::RefineTag;
        using SelectTag = typename DetectorType::SelectTag;
        DetectorType bd2{{64, 64}, 64};
        REQUIRE(bd2.intersect(r));
        REQUIRE(bd2.intersect(r1));
        REQUIRE(bd2.intersect(r2));
        REQUIRE(bd2.intersect(r3));
        REQUIRE(bd2.intersect(r4));
        REQUIRE(!bd2.intersect(r5));
        REQUIRE(!bd2.intersect(r6));
        REQUIRE(!bd2.intersect(r7));
        REQUIRE(!bd2.intersect(r8));
        REQUIRE(!bd2.intersect(r9));
        REQUIRE(!bd2.intersect(r10));
        REQUIRE(bd2(Intersect{}, r));
        REQUIRE(bd2(Intersect{}, r1));
        REQUIRE(bd2(Intersect{}, r2));
        REQUIRE(bd2(Intersect{}, r3));
        REQUIRE(bd2(Intersect{}, r4));
        REQUIRE(!bd2(Intersect{}, r5));
        REQUIRE(!bd2(Intersect{}, r6));
        REQUIRE(!bd2(Intersect{}, r7));
        REQUIRE(!bd2(Intersect{}, r8));
        REQUIRE(!bd2(Intersect{}, r9));
        REQUIRE(!bd2(Intersect{}, r10));
        REQUIRE(bd2(RefineTag{}, r));
        REQUIRE(bd2(RefineTag{}, r1));
        REQUIRE(bd2(RefineTag{}, r2));
        REQUIRE(bd2(RefineTag{}, r3));
        REQUIRE(bd2(RefineTag{}, r4));
        REQUIRE(!bd2(RefineTag{}, r5));
        REQUIRE(!bd2(RefineTag{}, r6));
        REQUIRE(!bd2(RefineTag{}, r7));
        REQUIRE(!bd2(RefineTag{}, r8));
        REQUIRE(!bd2(RefineTag{}, r9));
        REQUIRE(!bd2(RefineTag{}, r10));
        REQUIRE(bd2.overlap(r));
        REQUIRE(bd2.overlap(r1));
        REQUIRE(bd2.overlap(r2));
        REQUIRE(bd2.overlap(r3));
        REQUIRE(bd2.overlap(r4));
        REQUIRE(bd2.overlap(r5));
        REQUIRE(bd2.overlap(r6));
        REQUIRE(!bd2.overlap(r7));
        REQUIRE(!bd2.overlap(r8));
        REQUIRE(!bd2.overlap(r9));
        REQUIRE(!bd2.overlap(r10));
        REQUIRE(bd2.select(r));
        REQUIRE(bd2.select(r1));
        REQUIRE(bd2.select(r2));
        REQUIRE(bd2.select(r3));
        REQUIRE(bd2.select(r4));
        REQUIRE(bd2.select(r5));
        REQUIRE(bd2.select(r6));
        REQUIRE(!bd2.select(r7));
        REQUIRE(!bd2.select(r8));
        REQUIRE(!bd2.select(r9));
        REQUIRE(!bd2.select(r10));
        REQUIRE(bd2(Select{}, r));
        REQUIRE(bd2(Select{}, r1));
        REQUIRE(bd2(Select{}, r2));
        REQUIRE(bd2(Select{}, r3));
        REQUIRE(bd2(Select{}, r4));
        REQUIRE(bd2(Select{}, r5));
        REQUIRE(bd2(Select{}, r6));
        REQUIRE(!bd2(Select{}, r7));
        REQUIRE(!bd2(Select{}, r8));
        REQUIRE(!bd2(Select{}, r9));
        REQUIRE(!bd2(Select{}, r10));
        REQUIRE(bd2(SelectTag{}, r));
        REQUIRE(bd2(SelectTag{}, r1));
        REQUIRE(bd2(SelectTag{}, r2));
        REQUIRE(bd2(SelectTag{}, r3));
        REQUIRE(bd2(SelectTag{}, r4));
        REQUIRE(bd2(SelectTag{}, r5));
        REQUIRE(bd2(SelectTag{}, r6));
        REQUIRE(bd2(SelectTag{}, r7));
        REQUIRE(bd2(SelectTag{}, r8));
        REQUIRE(bd2(SelectTag{}, r9));
        REQUIRE(bd2(SelectTag{}, r10));
    }
}

TEMPLATE_LIST_TEST_CASE("BallDetector - 3D", "[Detectors][template]",
    test::IntTypes)
{
    using Ival = Interval<TestType>;
    refine::BallDetector<TestType, 3> bd3{{64, 64, 64}, 64};
    Region<TestType, 3> r{{Ival{0, 128}, Ival{0, 128}, Ival{0, 128}}};

    Region<TestType, 3> rc1{{Ival{0, 64}, Ival{0, 64}, Ival{0, 64}}};
    Region<TestType, 3> rc2{{Ival{0, 64}, Ival{0, 64}, Ival{64, 128}}};
    Region<TestType, 3> rc3{{Ival{0, 64}, Ival{64, 128}, Ival{0, 64}}};
    Region<TestType, 3> rc4{{Ival{0, 64}, Ival{64, 128}, Ival{64, 128}}};

    Region<TestType, 3> rc5{{Ival{64, 128}, Ival{0, 64}, Ival{0, 64}}};
    Region<TestType, 3> rc6{{Ival{64, 128}, Ival{0, 64}, Ival{64, 128}}};
    Region<TestType, 3> rc7{{Ival{64, 128}, Ival{64, 128}, Ival{0, 64}}};
    Region<TestType, 3> rc8{{Ival{64, 128}, Ival{64, 128}, Ival{64, 128}}};

    Region<TestType, 3> r5{{Ival{24, 32}, Ival{32, 64}, Ival{32, 96}}};
    Region<TestType, 3> r6{{Ival{32, 32}, Ival{32, 32}, Ival{32, 32}}};

    Region<TestType, 3> ro1{{Ival{16}, Ival{16}, Ival{16}}};
    Region<TestType, 3> ro2{{Ival{16}, Ival{16}, Ival{112, 128}}};
    Region<TestType, 3> ro3{{Ival{16}, Ival{112, 128}, Ival{16}}};
    Region<TestType, 3> ro4{{Ival{16}, Ival{112, 128}, Ival{112, 128}}};
    Region<TestType, 3> ro5{{Ival{112, 128}, Ival{16}, Ival{16}}};
    Region<TestType, 3> ro6{{Ival{112, 128}, Ival{16}, Ival{112, 128}}};
    Region<TestType, 3> ro7{{Ival{112, 128}, Ival{112, 128}, Ival{16}}};
    Region<TestType, 3> ro8{{Ival{112, 128}, Ival{112, 128}, Ival{112, 128}}};

    REQUIRE(bd3.intersect(r));
    REQUIRE(bd3.intersect(rc1));
    REQUIRE(bd3.intersect(rc2));
    REQUIRE(bd3.intersect(rc3));
    REQUIRE(bd3.intersect(rc4));
    REQUIRE(bd3.intersect(rc5));
    REQUIRE(bd3.intersect(rc6));
    REQUIRE(bd3.intersect(rc7));
    REQUIRE(bd3.intersect(rc8));
    REQUIRE(!bd3.intersect(r5));
    REQUIRE(!bd3.intersect(r6));
    REQUIRE(!bd3.intersect(ro1));
    REQUIRE(!bd3.intersect(ro2));
    REQUIRE(!bd3.intersect(ro3));
    REQUIRE(!bd3.intersect(ro4));
    REQUIRE(!bd3.intersect(ro5));
    REQUIRE(!bd3.intersect(ro6));
    REQUIRE(!bd3.intersect(ro7));
    REQUIRE(!bd3.intersect(ro8));
    REQUIRE(bd3(Intersect{}, r));
    REQUIRE(bd3(Intersect{}, rc1));
    REQUIRE(bd3(Intersect{}, rc2));
    REQUIRE(bd3(Intersect{}, rc3));
    REQUIRE(bd3(Intersect{}, rc4));
    REQUIRE(bd3(Intersect{}, rc5));
    REQUIRE(bd3(Intersect{}, rc6));
    REQUIRE(bd3(Intersect{}, rc7));
    REQUIRE(bd3(Intersect{}, rc8));
    REQUIRE(!bd3(Intersect{}, r5));
    REQUIRE(!bd3(Intersect{}, r6));
    REQUIRE(!bd3(Intersect{}, ro1));
    REQUIRE(!bd3(Intersect{}, ro2));
    REQUIRE(!bd3(Intersect{}, ro3));
    REQUIRE(!bd3(Intersect{}, ro4));
    REQUIRE(!bd3(Intersect{}, ro5));
    REQUIRE(!bd3(Intersect{}, ro6));
    REQUIRE(!bd3(Intersect{}, ro7));
    REQUIRE(!bd3(Intersect{}, ro8));
    REQUIRE(bd3.overlap(r));
    REQUIRE(bd3.overlap(rc1));
    REQUIRE(bd3.overlap(rc2));
    REQUIRE(bd3.overlap(rc3));
    REQUIRE(bd3.overlap(rc4));
    REQUIRE(bd3.overlap(rc5));
    REQUIRE(bd3.overlap(rc6));
    REQUIRE(bd3.overlap(rc7));
    REQUIRE(bd3.overlap(rc8));
    REQUIRE(bd3.overlap(r5));
    REQUIRE(bd3.overlap(r6));
    REQUIRE(!bd3.overlap(ro1));
    REQUIRE(!bd3.overlap(ro2));
    REQUIRE(!bd3.overlap(ro3));
    REQUIRE(!bd3.overlap(ro4));
    REQUIRE(!bd3.overlap(ro5));
    REQUIRE(!bd3.overlap(ro6));
    REQUIRE(!bd3.overlap(ro7));
    REQUIRE(!bd3.overlap(ro8));
    REQUIRE(bd3.select(r));
    REQUIRE(bd3.select(rc1));
    REQUIRE(bd3.select(rc2));
    REQUIRE(bd3.select(rc3));
    REQUIRE(bd3.select(rc4));
    REQUIRE(bd3.select(rc5));
    REQUIRE(bd3.select(rc6));
    REQUIRE(bd3.select(rc7));
    REQUIRE(bd3.select(rc8));
    REQUIRE(bd3.select(r5));
    REQUIRE(bd3.select(r6));
    REQUIRE(!bd3.select(ro1));
    REQUIRE(!bd3.select(ro2));
    REQUIRE(!bd3.select(ro3));
    REQUIRE(!bd3.select(ro4));
    REQUIRE(!bd3.select(ro5));
    REQUIRE(!bd3.select(ro6));
    REQUIRE(!bd3.select(ro7));
    REQUIRE(!bd3.select(ro8));
    REQUIRE(bd3(Select{}, r));
    REQUIRE(bd3(Select{}, rc1));
    REQUIRE(bd3(Select{}, rc2));
    REQUIRE(bd3(Select{}, rc3));
    REQUIRE(bd3(Select{}, rc4));
    REQUIRE(bd3(Select{}, rc5));
    REQUIRE(bd3(Select{}, rc6));
    REQUIRE(bd3(Select{}, rc7));
    REQUIRE(bd3(Select{}, rc8));
    REQUIRE(bd3(Select{}, r5));
    REQUIRE(bd3(Select{}, r6));
    REQUIRE(!bd3(Select{}, ro1));
    REQUIRE(!bd3(Select{}, ro2));
    REQUIRE(!bd3(Select{}, ro3));
    REQUIRE(!bd3(Select{}, ro4));
    REQUIRE(!bd3(Select{}, ro5));
    REQUIRE(!bd3(Select{}, ro6));
    REQUIRE(!bd3(Select{}, ro7));
    REQUIRE(!bd3(Select{}, ro8));
}
