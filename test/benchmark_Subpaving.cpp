#include <catch.hpp>

#include <exception>
#include <iostream>

#include <plsm/RenderSubpaving.h>
#include <plsm/Subpaving.h>
#include <plsm/TestingCommon.h>
#include <plsm/refine/BallDetector.h>
#include <plsm/refine/PolylineDetector.h>
#include <plsm/refine/RegionDetector.h>
using namespace plsm;

TEST_CASE("Subpaving 2D", "[Subpaving]")
{
	using namespace refine;
	using RegionType = typename Subpaving<int, 2>::RegionType;
	using Ival = Interval<int>;
	Region<int, 2> r{{Ival{-512, 512}, Ival{-512, 512}}};
	Subpaving<int, 2> s(r, {{{8, 4}, {4, 8}, {2, 2}}});

	SECTION("ball")
	{
		// BENCHMARK("refine: ball 2D")
		{
			using Tags = TagPair<Intersect, SelectAll>;
			s.refine(BallDetector<int, 2, Tags>{{0, 0, 0}, 500});
		};
		test::renderSubpaving(s);
	}

	SECTION("ball")
	{
		// BENCHMARK("refine: ball only 2D")
		{
			using Tags = TagPair<Intersect, Overlap>;
			s.refine(BallDetector<int, 2, Tags>{{0, 0, 0}, 500});
		};
		test::renderSubpaving(s);
	}

	SECTION("z-aligned")
	{
		std::vector<SpaceVector<int, 2>> rspecPoints(
			{{{0, 0}}, {{256, 128}}, {{384, 256}}, {{512, 512}}});

		// BENCHMARK("refine: polyline plus region 2D")
		{
			s.refine(PolylineDetector<int, 2>{rspecPoints});
			using Tags = TagPair<Overlap, SelectAll>;
			s.refine(RegionDetector<int, 2, Tags>{{Ival{0, 56}, Ival{0, 56}}});
		};
		test::renderSubpaving(s);
	}
}

TEST_CASE("Subpaving 3D", "[Subpaving]")
{
	using namespace refine;
	using RegionType = typename Subpaving<int, 3>::RegionType;
	using Ival = Interval<int>;
	RegionType r{{Ival{0, 512}, Ival{0, 512}, Ival{0, 512}}};
	Subpaving<int, 3> s(r, {{{2, 2, 2}}});
	std::vector<SpaceVector<int, 3>> rspecPoints;
	constexpr auto wild = wildcard<int>;

	SECTION("ball")
	{
		// BENCHMARK("refine: ball")
		{
			using Tags = TagPair<Intersect, Overlap>;
			s.refine(BallDetector<int, 3, Tags>{{256, 256, 256}, 128});
		};
		test::renderSubpaving(s);
	}

	SECTION("z-aligned")
	{
		rspecPoints.assign({{{0, 0, wild}}, {{256, 128, wild}},
			{{384, 256, wild}}, {{512, 512, wild}}});

		// BENCHMARK("refine: z-aligned polyline plus box")
		{
			s.refine(PolylineDetector<int, 3>{rspecPoints});
			Interval<int> ival{0, 56};
			using RegionDetector =
				RegionDetector<int, 3, TagPair<Overlap, SelectAll>>;
			s.refine(RegionDetector{{ival, ival, ival}});
		};
		test::renderSubpaving(s);
	}

	SECTION("x-aligned")
	{
		rspecPoints.assign({{{wild, 0, 0}}, {{wild, 256, 128}},
			{{wild, 384, 256}}, {{wild, 512, 512}}});

		// BENCHMARK("refine: x-aligned")
		{
			using Tags = TagPair<Intersect, SelectAll>;
			s.refine(PolylineDetector<int, 3, Tags>{rspecPoints});
		};
		test::renderSubpaving(s);
	}
}

TEST_CASE("Subpaving 2D(ish)", "[Subpaving]")
{
	using namespace refine;
	using RegionType = typename Subpaving<int, 3>::RegionType;
	using Ival = Interval<int>;
	RegionType r{{Ival{0, 512}, Ival{0, 512}, Ival{256, 257}}};
	Subpaving<int, 3> s(r, {{{2, 2, 1}}});
	std::vector<SpaceVector<int, 3>> rspecPoints;
	constexpr auto wild = wildcard<int>;

	SECTION("ball")
	{
		// BENCHMARK("refine: ball 2D-ish")
		{
			using BallDetector =
				BallDetector<int, 3, TagPair<Intersect, SelectAll>>;
			s.refine(BallDetector{{256, 256, 256}, 128});
		};
		test::renderSubpaving(s);
	}

	SECTION("z-aligned")
	{
		rspecPoints.assign({{{0, 0, wild}}, {{256, 128, wild}},
			{{384, 256, wild}}, {{512, 512, wild}}});

		// BENCHMARK("refine: z-aligned plus region 2D-ish")
		{
			s.refine(PolylineDetector<int, 3>{rspecPoints});
			Interval<int> ival{0, 56};
			using RegionDetector =
				RegionDetector<int, 3, TagPair<Overlap, SelectAll>>;
			s.refine(RegionDetector{{ival, ival, Interval<int>{0, 512}}});
		};
		test::renderSubpaving(s);
	}

	SECTION("x-aligned")
	{
		rspecPoints.assign({{{wild, 0, 0}}, {{wild, 256, 128}},
			{{wild, 384, 256}}, {{wild, 512, 512}}});

		// BENCHMARK("refine: x-aligned")
		{
			using Tags = TagPair<Intersect, SelectAll>;
			s.refine(PolylineDetector<int, 3, Tags>{rspecPoints});
		};
		test::renderSubpaving(s);
	}
}

TEST_CASE("Subpaving with XRN Defaults", "[Subpaving]")
{
	using RegionType = typename Subpaving<int, 3>::RegionType;
	using Ival = Interval<int>;
	RegionType r{{Ival{0, 5120}, Ival{0, 4096}, Ival{0, 12}}};
	Subpaving<int, 3> s(r, {{{10, 8, 3}}, {{8, 8, 4}}});
	std::vector<SpaceVector<int, 3>> rspecPoints;
	constexpr auto wild = wildcard<int>;
	rspecPoints.push_back({{wild, wild, 3}});
	// BENCHMARK("refine: XRN Default")
	// {
	//     s.refine(refine::PolylineDetector<int, 3>{rspecPoints});
	// };
}
