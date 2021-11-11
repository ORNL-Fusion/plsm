#pragma once

#include <plsm/MultiIndex.h>
#include <plsm/Utility.h>
#include <plsm/refine/Detector.h>

namespace plsm
{
namespace detail
{
struct CountNewItemsTag
{
};

Kokkos::RangePolicy<CountNewItemsTag>
countNewItemsPolicy(IdType sz)
{
	return {0, sz};
}

struct FindIndicesTag
{
};

struct RefineTileTag
{
};

Kokkos::RangePolicy<RefineTileTag>
refineTilePolicy(IdType sz)
{
	return {0, sz};
}

template <typename TData>
class RefinerFunctor
{
public:
	using RegionType = typename TData::SubpavingType::RegionType;
	using BoolVec = typename TData::DetectorType::template BoolVec<RegionType>;
	using ScalarType = typename TData::SubpavingType::ScalarType;
	using ZoneType = typename TData::SubpavingType::ZoneType;
	using TileType = typename TData::SubpavingType::TileType;
	using SubdivisionRatioType = ::plsm::SubdivisionRatio<TData::subpavingDim>;
	using SubdivisionInfoType = typename TData::SubdivisionInfoType;
	using DetectorType = typename TData::DetectorType;

	RefinerFunctor(const TData& data) : _data(data)
	{
	}

	KOKKOS_INLINE_FUNCTION
	SubdivisionRatioType
	getSubdivisionRatio(std::size_t level, IdType tileIndex) const
	{
		SubdivisionRatioType ret =
			_data.subdivisionInfos.d_view[level].getRatio();
		for (DimType i = 0; i < _data.subpavingDim; ++i) {
			if (!_data.enableRefine[i].test(static_cast<unsigned>(tileIndex))) {
				ret[i] = 1;
			}
		}
		return ret;
	}

	KOKKOS_INLINE_FUNCTION
	RegionType
	getSubZoneRegion(const ZoneType& zone, IdType subZoneLocalId,
		const SubdivisionInfoType& subdivInfo) const
	{
		using IntervalType = typename RegionType::IntervalType;

		constexpr auto subpavingDim = TData::subpavingDim;

		MultiIndex<subpavingDim> mId = subdivInfo.getMultiIndex(subZoneLocalId);

		const auto& zoneRegion = zone.getRegion();
		RegionType ret;
		for (auto i : makeIntervalRange(subpavingDim)) {
			const auto& ival = zoneRegion[i];
			auto delta = ival.length() / subdivInfo.getRatio()[i];
			ret[i] = IntervalType{
				ival.begin() + static_cast<ScalarType>(mId[i] * delta),
				ival.begin() + static_cast<ScalarType>((mId[i] + 1) * delta)};
		}
		return ret;
	}

	KOKKOS_INLINE_FUNCTION
	IdType
	countSelectSubZones(IdType index, const ZoneType& zone) const
	{
		SubdivisionInfoType info(getSubdivisionRatio(zone.getLevel(), index));
		auto numSubRegions = info.getRatio().getProduct();
		auto selected =
			Kokkos::subview(_data.selectedSubZones, index, Kokkos::ALL);
		IdType count = 0;
		for (auto i : makeIntervalRange(numSubRegions)) {
			auto subRegion = getSubZoneRegion(zone, i, info);
			if (_data.detector(DetectorType::selectTag, subRegion)) {
				selected(count) = i;
				++count;
			}
		}
		return count;
	}

	KOKKOS_INLINE_FUNCTION
	void
	operator()(CountNewItemsTag, IdType index, ItemTotals& runningTotals) const
	{
		const auto& tile = _data.tiles(index);
		auto zoneId = tile.getOwningZoneIndex();
		IdType count = 0;
		auto& zone = _data.zones(zoneId);
		auto level = zone.getLevel();
		if (level < _data.targetDepth) {
			BoolVec enable{};
			if (_data.detector(
					DetectorType::refineTag, tile.getRegion(), enable)) {
				for (DimType i = 0; i < _data.subpavingDim; ++i) {
					if (enable[i]) {
						_data.enableRefine[i].set(static_cast<unsigned>(index));
					}
					else {
						_data.enableRefine[i].reset(
							static_cast<unsigned>(index));
					}
				}
				count = countSelectSubZones(index, zone);
			}
		}
		_data.newZoneCounts(index) = count;
		if (count > 0) {
			runningTotals.zones += count;
			runningTotals.tiles += count - 1;
		}
	}

	KOKKOS_INLINE_FUNCTION
	void
	operator()(RefineTileTag, IdType index) const
	{
		auto newZones = _data.newZoneCounts(index);
		if (newZones == 0) {
			return;
		}

		auto& tile = _data.tiles(index);
		auto ownerZoneId = tile.getOwningZoneIndex();
		auto& ownerZone = _data.zones(ownerZoneId);
		auto level = ownerZone.getLevel();
		auto newLevel = level + 1;
		auto info = SubdivisionInfoType(getSubdivisionRatio(level, index));

		// Create first new zone, replace current tile and associate
		auto subZoneBeginId = _data.numZones + _data.subZoneStarts(index);
		_data.zones(subZoneBeginId) = ZoneType{
			getSubZoneRegion(ownerZone, _data.selectedSubZones(index, 0), info),
			newLevel, ownerZoneId};
		_data.zones(subZoneBeginId).setTileIndex(index);
		tile =
			TileType{_data.zones(subZoneBeginId).getRegion(), subZoneBeginId};

		// Create and associate remaining zones and tiles
		auto tileBeginId = _data.numTiles + _data.newTileStarts(index);
		for (IdType i = 1; i < newZones; ++i) {
			auto zoneId = subZoneBeginId + i;
			auto tileId = tileBeginId + i - 1;
			_data.zones(zoneId) =
				ZoneType{getSubZoneRegion(
							 ownerZone, _data.selectedSubZones(index, i), info),
					newLevel, ownerZoneId};
			_data.zones(zoneId).setTileIndex(tileId);
			_data.tiles(tileId) =
				TileType{_data.zones(zoneId).getRegion(), zoneId};
		}

		ownerZone.removeTile();
		ownerZone.setSubZoneIndices(
			{subZoneBeginId, subZoneBeginId + newZones});
	}

protected:
	TData _data;
};

template <typename TSubpaving, typename TDetector>
Refiner<TSubpaving, TDetector>::Refiner(
	SubpavingType& subpaving, const DetectorType& detector) :
	_subpaving(subpaving),
	_data{subpaving._zones.d_view, subpaving._tiles.d_view,
		subpaving._subdivisionInfos, detector,
		(detector.depth() == detector.fullDepth) ?
			subpaving._subdivisionInfos.h_view.size() :
			detector.depth()}
{
}

template <typename TSubpaving, typename TDetector>
void
Refiner<TSubpaving, TDetector>::operator()()
{
	std::size_t currLevel = 0;
	for (; currLevel < _data.targetDepth; ++currLevel) {
		countNewItems();

		if (_data.newItemTotals.zones == 0) {
			break;
		}

		findNewItemIndices();

		assignNewItems();
	}

	_subpaving._zones.d_view = _data.zones;
	_subpaving._zones.modify_device();
	_subpaving._tiles.d_view = _data.tiles;
	_subpaving._tiles.modify_device();
	_subpaving._refinementDepth = currLevel;
}

template <typename TSubpaving, typename TDetector>
void
Refiner<TSubpaving, TDetector>::countNewItems()
{
	auto numTiles = _data.tiles.size();
	_data.newZoneCounts = Kokkos::View<IdType*>(
		Kokkos::ViewAllocateWithoutInitializing{"New Zone Counts"}, numTiles);
	auto numSubZones =
		_data.subdivisionInfos.h_view(_data.currLevel).getRatio().getProduct();
	_data.selectedSubZones = Kokkos::View<IdType**>(
		Kokkos::ViewAllocateWithoutInitializing{"Selected Sub-Zones"}, numTiles,
		numSubZones);
	std::for_each(begin(_data.enableRefine), end(_data.enableRefine),
		[numTiles](auto&& bitset) {
			using Bitset = std::remove_reference_t<decltype(bitset)>;
			bitset = Bitset(static_cast<unsigned>(numTiles));
		});
	ItemTotals counts{};
	Kokkos::parallel_reduce("CountNewItemsFromTile",
		countNewItemsPolicy(numTiles), RefinerFunctor<Data>{_data}, counts);
	Kokkos::fence();
	_data.newItemTotals = counts;
}

template <typename TSubpaving, typename TDetector>
void
Refiner<TSubpaving, TDetector>::findNewItemIndices()
{
	auto numTiles = _data.tiles.size();
	auto subZoneStarts = Kokkos::View<IdType*>(
		Kokkos::ViewAllocateWithoutInitializing{"SubZone Start Ids"}, numTiles);
	auto newTileStarts = Kokkos::View<IdType*>(
		Kokkos::ViewAllocateWithoutInitializing{"Tile Start Ids"}, numTiles);
	auto newZoneCounts = _data.newZoneCounts;

	// Initialize starts
	Kokkos::parallel_for(
		"InitializeNewItemStarts", numTiles, KOKKOS_LAMBDA(IdType i) {
			auto newZoneCount = newZoneCounts(i);
			subZoneStarts(i) = newZoneCount;
			newTileStarts(i) = (newZoneCount == 0) ? 0 : newZoneCount - 1;
		});

	ItemTotals totals{};
	Kokkos::parallel_scan(
		"ScanNewItemStarts", numTiles,
		KOKKOS_LAMBDA(IdType i, ItemTotals & update, const bool finalPass) {
			const auto tmpZones = subZoneStarts(i);
			const auto tmpTiles = newTileStarts(i);
			if (finalPass) {
				subZoneStarts(i) = update.zones;
				newTileStarts(i) = update.tiles;
			}
			update.zones += tmpZones;
			update.tiles += tmpTiles;
		},
		totals);

	Kokkos::fence();
	_data.subZoneStarts = subZoneStarts;
	_data.newTileStarts = newTileStarts;
}

template <typename TSubpaving, typename TDetector>
void
Refiner<TSubpaving, TDetector>::assignNewItems()
{
	Kokkos::resize(_data.zones, _data.numZones + _data.newItemTotals.zones);
	Kokkos::resize(_data.tiles, _data.numTiles + _data.newItemTotals.tiles);

	Kokkos::parallel_for("RefineTile", refineTilePolicy(_data.numTiles),
		RefinerFunctor<Data>{_data});
	Kokkos::fence();

	_data.numZones = _data.zones.size();
	_data.numTiles = _data.tiles.size();
}
} // namespace detail
} // namespace plsm
