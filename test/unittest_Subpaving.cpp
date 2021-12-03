#include <catch.hpp>

#include <exception>
#include <iostream>

#include <plsm/PrintSubpaving.h>
#include <plsm/RenderSubpaving.h>
#include <plsm/Subpaving.h>
#include <plsm/TestingCommon.h>
#include <plsm/refine/RegionDetector.h>
using namespace plsm;

namespace plsm::test
{
template <typename TSubpaving>
struct SubpavingTester
{
	using Ratio = SubdivisionRatio<TSubpaving::dimension()>;

	void
	checkRatios(const std::vector<Ratio>& ratios)
	{
		auto infos = subpaving.makeMirrorCopy()._subdivisionInfos;
		REQUIRE(infos.size() == ratios.size());
	}

	TSubpaving subpaving;
};

template <typename TSubpaving>
SubpavingTester(const TSubpaving&) -> SubpavingTester<TSubpaving>;
} // namespace plsm::test

TEMPLATE_LIST_TEST_CASE(
	"Process Subdivision Ratios", "[Subpaving][template]", test::IntTypes)
{
	auto subpaving = Subpaving<TestType, 2>({{{0, 100}, {0, 100}}}, {{{5, 5}}});
	test::SubpavingTester{subpaving}.checkRatios(
		{{{5, 5}, {5, 5}, {2, 2}, {2, 2}}});

	subpaving = Subpaving<TestType, 2>({{{0, 250}, {0, 50}}}, {{{5, 5}}});
	test::SubpavingTester{subpaving}.checkRatios(
		{{{5, 5}, {5, 5}, {5, 2}, {2, 2}}});

	subpaving = Subpaving<TestType, 2>({{{10, 20}, {25, 35}}}, {{{5, 5}}});
	test::SubpavingTester{subpaving}.checkRatios({{{5, 5}, {2, 2}}});

	subpaving = Subpaving<TestType, 2>({{{0, 300}, {0, 275}}}, {{{5, 5}}});
	test::SubpavingTester{subpaving}.checkRatios(
		{{{5, 5}, {5, 5}, {4, 11}, {3, 1}}});
}

TEMPLATE_LIST_TEST_CASE(
	"Subpaving Basic", "[Subpaving][template]", test::IntTypes)
{
	using SubpavingType = Subpaving<TestType, 3>;
	using RegionType = typename SubpavingType::RegionType;
	using PointType = typename SubpavingType::PointType;
	using Ival = typename RegionType::IntervalType;
	RegionType r{{Ival{0, 4}, Ival{0, 4}, Ival{0, 4}}};
	SubpavingType sp(r, {{{2, 2, 2}}});
	SECTION("Uniform Refinement")
	{
		using RegionDetector = refine::RegionDetector<TestType, 3,
			refine::TagPair<refine::Overlap, refine::SelectAll>>;
		sp.refine(RegionDetector{sp.getLatticeRegion()});
		REQUIRE(sp.getTiles().extent(0) == 64);

		auto sph = sp.makeMirrorCopy();
		REQUIRE(sph.findTileId({3, 3, 3}) == 63);

		using Range3D = Kokkos::MDRangePolicy<Kokkos::Rank<3>>;
		std::size_t errors = 0;
		auto tiles = sp.getTiles();

		Kokkos::parallel_reduce(
			Range3D({0, 0, 0}, {4, 4, 4}),
			KOKKOS_LAMBDA(
				TestType i, TestType j, TestType k, std::size_t & running) {
				auto tileId = sp.findTileId(PointType{i, j, k});
				if (tileId == invalid<std::size_t>) {
					++running;
				}
			},
			errors);
		REQUIRE(errors == 0);

		errors = 0;
		Kokkos::parallel_reduce(
			Range3D({0, 0, 0}, {4, 4, 4}),
			KOKKOS_LAMBDA(
				TestType i, TestType j, TestType k, std::size_t & running) {
				PointType p({i, j, k});
				auto tileId = sp.findTileId(p);
				if (tiles(tileId).getRegion().getOrigin() != p) {
					++running;
				}
			},
			errors);
		REQUIRE(errors == 0);
	}
}
