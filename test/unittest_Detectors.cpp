#include <catch.hpp>

#include <type_traits>

#include <plsm/Region.h>
#include <plsm/TestingCommon.h>
#include <plsm/refine/BallDetector.h>
#include <plsm/refine/MultiDetector.h>
#include <plsm/refine/PolylineDetector.h>
#include <plsm/refine/RegionDetector.h>
using namespace plsm;

TEMPLATE_LIST_TEST_CASE(
	"PolylineDetector - 3D", "[Detectors][template]", test::IntTypes)
{
	// using Ival = Interval<TestType>;
	// refine::PolylineDetector<TestType, 3> pd3;
	// TODO
}

TEMPLATE_LIST_TEST_CASE(
	"RegionDetector - 2D", "[Detectors][template]", test::IntTypes)
{
	using Ival = Interval<TestType>;
	Region<TestType, 2> r{{Ival{128}, Ival{128}}};

	using DetectorType = refine::RegionDetector<TestType, 2>;
	// using RefineTag = typename DetectorType::RefineTag;
	// using SelectTag = typename DetectorType::SelectTag;
	DetectorType rd2{{Ival{32, 96}, Ival{32, 96}}};

	REQUIRE(rd2.overlap(r));

	// TODO: Need to test intersect
}

TEMPLATE_LIST_TEST_CASE(
	"RegionDetector - 3D", "[Detectors][template]", test::IntTypes)
{
	using Ival = Interval<TestType>;
	refine::RegionDetector<TestType, 3> rd3{{Ival{56}, Ival{56}, Ival{56}}};
}

TEMPLATE_LIST_TEST_CASE(
	"BallDetector - 2D", "[Detectors][template]", test::IntTypes)
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

	SECTION("Default Mode")
	{
		using refine::Intersect;
		using refine::Overlap;
		using refine::TagPair;
		using DetectorType =
			refine::BallDetector<TestType, 2, TagPair<Intersect, Overlap>>;
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
		REQUIRE(bd2(Overlap{}, r));
		REQUIRE(bd2(Overlap{}, r1));
		REQUIRE(bd2(Overlap{}, r2));
		REQUIRE(bd2(Overlap{}, r3));
		REQUIRE(bd2(Overlap{}, r4));
		REQUIRE(bd2(Overlap{}, r5));
		REQUIRE(bd2(Overlap{}, r6));
		REQUIRE(!bd2(Overlap{}, r7));
		REQUIRE(!bd2(Overlap{}, r8));
		REQUIRE(!bd2(Overlap{}, r9));
		REQUIRE(!bd2(Overlap{}, r10));
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

	SECTION("Overlap Mode")
	{
		using refine::Intersect;
		using refine::Overlap;
		using refine::Select;
		using refine::SelectAll;
		using refine::TagPair;
		using DetectorType =
			refine::BallDetector<TestType, 2, TagPair<Overlap, SelectAll>>;
		using RefineTag = typename DetectorType::RefineTag;
		using SelectTag = typename DetectorType::SelectTag;
		static_assert(std::is_same<SelectTag, SelectAll>::value, "");
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
		using refine::Intersect;
		using refine::Select;
		using refine::SelectAll;
		using refine::TagPair;
		using DetectorType =
			refine::BallDetector<TestType, 2, TagPair<Intersect, SelectAll>>;
		DetectorType bd2{{64, 64}, 64};
		constexpr auto refineTag = bd2.refineTag;
		constexpr auto selectTag = bd2.selectTag;
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
		REQUIRE(bd2(refineTag, r));
		REQUIRE(bd2(refineTag, r1));
		REQUIRE(bd2(refineTag, r2));
		REQUIRE(bd2(refineTag, r3));
		REQUIRE(bd2(refineTag, r4));
		REQUIRE(!bd2(refineTag, r5));
		REQUIRE(!bd2(refineTag, r6));
		REQUIRE(!bd2(refineTag, r7));
		REQUIRE(!bd2(refineTag, r8));
		REQUIRE(!bd2(refineTag, r9));
		REQUIRE(!bd2(refineTag, r10));
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
		REQUIRE(bd2(selectTag, r));
		REQUIRE(bd2(selectTag, r1));
		REQUIRE(bd2(selectTag, r2));
		REQUIRE(bd2(selectTag, r3));
		REQUIRE(bd2(selectTag, r4));
		REQUIRE(bd2(selectTag, r5));
		REQUIRE(bd2(selectTag, r6));
		REQUIRE(bd2(selectTag, r7));
		REQUIRE(bd2(selectTag, r8));
		REQUIRE(bd2(selectTag, r9));
		REQUIRE(bd2(selectTag, r10));
	}
}

TEMPLATE_LIST_TEST_CASE(
	"BallDetector - 3D", "[Detectors][template]", test::IntTypes)
{
	using refine::Intersect;
	using refine::Overlap;
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
	REQUIRE(bd3(Overlap{}, r));
	REQUIRE(bd3(Overlap{}, rc1));
	REQUIRE(bd3(Overlap{}, rc2));
	REQUIRE(bd3(Overlap{}, rc3));
	REQUIRE(bd3(Overlap{}, rc4));
	REQUIRE(bd3(Overlap{}, rc5));
	REQUIRE(bd3(Overlap{}, rc6));
	REQUIRE(bd3(Overlap{}, rc7));
	REQUIRE(bd3(Overlap{}, rc8));
	REQUIRE(bd3(Overlap{}, r5));
	REQUIRE(bd3(Overlap{}, r6));
	REQUIRE(!bd3(Overlap{}, ro1));
	REQUIRE(!bd3(Overlap{}, ro2));
	REQUIRE(!bd3(Overlap{}, ro3));
	REQUIRE(!bd3(Overlap{}, ro4));
	REQUIRE(!bd3(Overlap{}, ro5));
	REQUIRE(!bd3(Overlap{}, ro6));
	REQUIRE(!bd3(Overlap{}, ro7));
	REQUIRE(!bd3(Overlap{}, ro8));
}

TEMPLATE_LIST_TEST_CASE(
	"MultiDetector", "[Detectors][template]", test::IntTypes)
{
	using namespace refine;
	using PD = PolylineDetector<TestType, 2>;
	using Tags = TagPair<Overlap, SelectAll>;
	using RD = RegionDetector<TestType, 2, Tags>;
	auto md = makeMultiDetector(
		PD{{{{0, 0}}, {{256, 128}}, {{384, 256}}, {{512, 512}}}},
		RD{{{0, 56}, {0, 56}}});

	Region<TestType, 2> r1{{{0, 16}, {0, 16}}};
	Region<TestType, 2> r2{{{250, 300}, {100, 150}}};
	Region<TestType, 2> r3{{{450, 500}, {0, 50}}};
	typename PD::template BoolVec<Region<TestType, 2>> res;
	REQUIRE(md.refine(r1, res));
	REQUIRE(md.refine(r2, res));
	REQUIRE(!md.refine(r3, res));

	REQUIRE(md.select(r1));
	REQUIRE(md.select(r2));
	REQUIRE(!md.select(r3));
}
