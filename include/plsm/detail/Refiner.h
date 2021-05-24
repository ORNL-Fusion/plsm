#pragma once

#include <utility>
#include <vector>

#include <Kokkos_Bitset.hpp>
#include <Kokkos_Vector.hpp>

#include <plsm/ContextUtility.h>
#include <plsm/SpaceVector.h>
#include <plsm/Subpaving.h>
#include <plsm/Utility.h>
#include <plsm/detail/SubdivisionInfo.h>

namespace plsm
{
namespace detail
{
/*!
 * @brief Refiner handles the refinement and selection of the Subpaving tiles
 */
template <typename TSubpaving, typename TDetector>
class Refiner
{
public:
	static_assert(IsSubpaving<TSubpaving>::value, "");
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

	struct NewItemTotals
	{
		IdType zones = 0;
		IdType tiles = 0;

		NewItemTotals() = default;
		NewItemTotals(const NewItemTotals&) = default;

		KOKKOS_INLINE_FUNCTION
		volatile NewItemTotals&
		operator+=(const volatile NewItemTotals& other) volatile
		{
			zones += other.zones;
			tiles += other.tiles;
			return *this;
		}
	};

	KOKKOS_INLINE_FUNCTION
	IdType
	countSelectSubZones(IdType index, const ZoneType& zone) const;

	KOKKOS_INLINE_FUNCTION
	void
	countSelectNewItemsFromTile(
		IdType index, NewItemTotals& runningTotals) const;

	void
	countNewZonesAndTiles();

	void
	findNewItemIndices();

	KOKKOS_INLINE_FUNCTION
	void
	refineTile(IdType index) const;

	void
	assignNewZonesAndTiles();

	KOKKOS_INLINE_FUNCTION
	RegionType
	getSubZoneRegion(const ZoneType& zone, IdType subZoneLocalId,
		const SubdivisionInfoType& subdivInfo) const;

	KOKKOS_INLINE_FUNCTION
	SubdivisionRatioType
	getSubdivisionRatio(std::size_t level, IdType tileIndex) const;

protected:
	template <typename, DimType, typename, typename>
	friend class ::plsm::Subpaving;

	Refiner(SubpavingType& subpaving, DetectorType detector);

protected:
	SubpavingType& _subpaving;
	using ZonesView = typename SubpavingType::template ZonesView<OnDevice>;
	ZonesView _zones;
	using TilesView = typename SubpavingType::template TilesView<OnDevice>;
	TilesView _tiles;

	Kokkos::DualView<SubdivisionInfoType*> _subdivisionInfos;

	std::size_t _currLevel;
	std::size_t _targetDepth;

	DetectorType _detector;

	using DefaultExecSpace =
		typename Kokkos::View<int*>::traits::execution_space;
	Kokkos::Array<Kokkos::Bitset<DefaultExecSpace>, subpavingDim> _enableRefine;

	Kokkos::View<IdType**> _selectedSubZones;

	NewItemTotals _newItemTotals{};
	IdType _numTiles;
	IdType _numZones;
	Kokkos::View<IdType*> _newZoneCounts;
	Kokkos::View<IdType*> _subZoneStarts;
	Kokkos::View<IdType*> _newTileStarts;
};
} // namespace detail
} // namespace plsm

#include <plsm/detail/Refiner.inl>
