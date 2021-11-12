#include <catch.hpp>

#include <exception>
#include <iostream>

#include <plsm/RenderSubpaving.h>
#include <plsm/Subpaving.h>
#include <plsm/TestingCommon.h>
#include <plsm/refine/BallDetector.h>
#include <plsm/refine/MultiDetector.h>
#include <plsm/refine/PolylineDetector.h>
#include <plsm/refine/RegionDetector.h>
using namespace plsm;

TEST_CASE("Subpaving 2D", "[Subpaving]")
{
	using namespace refine;
	using Ival = Interval<int>;
	Subpaving<int, 2> s(
		{{{-512, 512}, {-512, 512}}}, {{/*{8, 4}, {4, 8}, */ {2, 2}}});

	SECTION("ball")
	{
		BENCHMARK("refine: ball 2D")
		{
			using Tags = TagPair<Intersect, SelectAll>;
			s.refine(BallDetector<int, 2, Tags>{{0, 0}, 500});
		};
		test::renderSubpaving(s);
	}

	SECTION("ball")
	{
		BENCHMARK("refine: ball only 2D")
		{
			using Tags = TagPair<Intersect, Overlap>;
			s.refine(BallDetector<int, 2, Tags>{{0, 0}, 500});
		};
		test::renderSubpaving(s);
	}

	SECTION("z-aligned")
	{
		Subpaving<int, 2> spv({{{0, 512}, {0, 512}}}, {{{2, 2}}});
		std::vector<SpaceVector<int, 2>> rspecPoints(
			{{{0, 0}}, {{256, 128}}, {{384, 256}}, {{512, 512}}});

		BENCHMARK("refine: polyline plus region 2D")
		{
			using PD = PolylineDetector<int, 2>;
			using Tags = TagPair<Overlap, SelectAll>;
			using RD = RegionDetector<int, 2, Tags>;
			spv.refine(makeMultiDetector(
				PD{{{{0, 0}}, {{256, 128}}, {{384, 256}}, {{512, 512}}}},
				RD{{Ival{0, 56}, Ival{0, 56}}}));
		};
		std::cout << "z-aligned\n";
		test::renderSubpaving(spv);
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
		BENCHMARK("refine: ball")
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

		BENCHMARK("refine: z-aligned polyline plus box")
		{
			s.refine(PolylineDetector<int, 3>{rspecPoints});
			Interval<int> ival{0, 56};
			using RegionDetector =
				RegionDetector<int, 3, TagPair<Overlap, SelectAll>>;
			s.refine(RegionDetector{{ival, ival, ival}});
		};
		test::renderSubpaving(s);
	}

	SECTION("z-aligned, multi")
	{
		rspecPoints.assign({{{0, 0, wild}}, {{256, 128, wild}},
			{{384, 256, wild}}, {{512, 512, wild}}});

		BENCHMARK("refine: z-aligned polyline plus box")
		{
			// s.refine(PolylineDetector<int, 3>{rspecPoints});
			Interval<int> ival{0, 56};
			using RegionDetector =
				RegionDetector<int, 3, TagPair<Overlap, SelectAll>>;
			// s.refine(RegionDetector{{ival, ival, ival}});
			s.refine(makeMultiDetector(PolylineDetector<int, 3>{rspecPoints},
				RegionDetector{{ival, ival, ival}}));
		};
		test::renderSubpaving(s);
	}

	SECTION("x-aligned")
	{
		rspecPoints.assign({{{wild, 0, 0}}, {{wild, 256, 128}},
			{{wild, 384, 256}}, {{wild, 512, 512}}});

		BENCHMARK("refine: x-aligned")
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
		BENCHMARK("refine: ball 2D-ish")
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

		BENCHMARK("refine: z-aligned plus region 2D-ish")
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

		BENCHMARK("refine: x-aligned")
		{
			using Tags = TagPair<Intersect, SelectAll>;
			s.refine(PolylineDetector<int, 3, Tags>{rspecPoints});
		};
		test::renderSubpaving(s);
	}
}

TEST_CASE("Subpaving with XRN Defaults", "[Subpaving][XRN]")
{
	using RegionType = typename Subpaving<int, 3>::RegionType;
	using Ival = Interval<int>;
	RegionType r{{Ival{0, 10240}, Ival{0, 8192}, Ival{0, 16}}};
	Subpaving<int, 3> s(r, {{{10, 8, 2}}, {{2, 2, 2}}});
	std::vector<SpaceVector<int, 3>> rspecPoints;
	constexpr auto wild = wildcard<int>;
	// rspecPoints.push_back({{wild, wild, 3}});
	rspecPoints.push_back({{0, 0, wild}});
	rspecPoints.push_back({{1000, 500, wild}});
	rspecPoints.push_back({{10240, 8192, wild}});
	BENCHMARK("refine: XRN Default")
	{
		s.refine(refine::PolylineDetector<int, 3>{rspecPoints});
	};
	test::renderSubpaving(s);
}
