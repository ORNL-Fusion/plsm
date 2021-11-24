#pragma once

#include <utility>
#include <vector>

#include <Kokkos_Bitset.hpp>

#include <plsm/SpaceVector.h>
#include <plsm/Subpaving.h>
#include <plsm/Utility.h>
#include <plsm/detail/SubdivisionInfo.h>

namespace plsm
{
namespace detail
{
struct ItemTotals
{
	IdType zones{0};
	IdType tiles{0};

	KOKKOS_INLINE_FUNCTION
	volatile ItemTotals&
	operator+=(const volatile ItemTotals& other) volatile
	{
		zones += other.zones;
		tiles += other.tiles;
		return *this;
	}
};

template <typename TSubpaving, typename TDetector>
struct RefinerData
{
	static_assert(IsSubpaving<TSubpaving>{});

	using SubpavingType = TSubpaving;
	using ZoneType = typename SubpavingType::ZoneType;
	using TileType = typename SubpavingType::TileType;
	using ZonesView = typename SubpavingType::ZonesView;
	using TilesView = typename SubpavingType::TilesView;

	static constexpr DimType subpavingDim = SubpavingType::dimension();

	using SubdivisionInfoType = SubdivisionInfo<subpavingDim>;

	using DetectorType = TDetector;

	ZonesView zones;
	TilesView tiles;

	Kokkos::View<SubdivisionInfoType*> subdivisionInfos;

	DetectorType detector;

	std::size_t targetDepth;
	std::size_t currLevel{};

	Kokkos::Array<Kokkos::Bitset<DefaultExecSpace>, subpavingDim>
		enableRefine{};

	Kokkos::View<IdType**> selectedSubZones{};

	Kokkos::View<IdType*> newZoneCounts{};
	Kokkos::View<IdType*> subZoneStarts{};
	Kokkos::View<IdType*> newTileStarts{};

	ItemTotals newItemTotals{};
	IdType numZones{zones.size()};
	IdType numTiles{tiles.size()};
};

/*!
 * @brief Refiner handles the refinement and selection of the Subpaving tiles
 */
template <typename TSubpaving, typename TDetector>
class Refiner
{
public:
	using SubpavingType = TSubpaving;
	using ScalarType = typename SubpavingType::ScalarType;
	using ZoneType = typename SubpavingType::ZoneType;
	using TileType = typename SubpavingType::TileType;
	using RegionType = typename SubpavingType::RegionType;
	using DetectorType = TDetector;

private:
	static constexpr DimType subpavingDim = SubpavingType::dimension();

public:
	using SubdivisionRatioType = ::plsm::SubdivisionRatio<subpavingDim>;
	using SubdivisionInfoType = SubdivisionInfo<subpavingDim>;

	void
	operator()();

	void
	countNewItems();

	void
	findNewItemIndices();

	void
	assignNewItems();

protected:
	template <typename, DimType, typename, typename, typename>
	friend class ::plsm::Subpaving;

	Refiner(SubpavingType& subpaving, const DetectorType& detector);

protected:
	SubpavingType& _subpaving;
	typename Kokkos::View<SubdivisionInfoType*>::HostMirror _subdivInfoMirror;

	RefinerData<TSubpaving, TDetector> _data;
};
} // namespace detail
} // namespace plsm

#include <plsm/detail/Refiner.inl>
