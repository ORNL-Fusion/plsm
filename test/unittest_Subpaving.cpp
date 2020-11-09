#include <catch.hpp>

#include <exception>
#include <iostream>

#include <plsm/PrintSubpaving.h>
#include <plsm/RenderSubpaving.h>
#include <plsm/Subpaving.h>
#include <plsm/TestingCommon.h>
#include <plsm/refine/RegionDetector.h>
using namespace plsm;

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
		REQUIRE(sp.getTiles(onDevice).extent(0) == 64);
		auto spMemSz = sp.getDeviceMemorySize();
		std::cout << "Mem Size: " << spMemSz << '\n';
		sp.syncTiles(onHost);
		REQUIRE(sp.getTiles().extent(0) == 64);
		REQUIRE(sp.findTileId({3, 3, 3}) == invalid<std::size_t>);
		sp.syncAll(onHost);
		REQUIRE(sp.findTileId({3, 3, 3}) == 63);

		using Range3D = Kokkos::MDRangePolicy<Kokkos::Rank<3>>;
		std::size_t errors = 0;
		auto tiles = sp.getTiles(onDevice);

		Kokkos::parallel_reduce(
			Range3D({0, 0, 0}, {4, 4, 4}),
			KOKKOS_LAMBDA(
				TestType i, TestType j, TestType k, std::size_t & running) {
				auto tileId = sp.findTileId(PointType{i, j, k}, onDevice);
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
				auto tileId = sp.findTileId(p, onDevice);
				if (tiles(tileId).getRegion().getOrigin() != p) {
					++running;
				}
			},
			errors);
		REQUIRE(errors == 0);
	}
}
