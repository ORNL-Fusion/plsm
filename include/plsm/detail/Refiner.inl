#pragma once

#include <plsm/MultiIndex.h>
#include <plsm/Utility.h>
#include <plsm/refine/Detector.h>

namespace plsm
{
namespace detail
{
using AllocNoInit = Kokkos::ViewAllocateWithoutInitializing;

template <typename TData>
KOKKOS_INLINE_FUNCTION
SubdivisionRatio<TData::subpavingDim>
getSubdivisionRatio(const TData& data, std::size_t level, IdType tileIndex)
{
	auto ret = data.subdivisionInfos.d_view[level].getRatio();
	for (DimType i = 0; i < data.subpavingDim; ++i) {
		if (!data.enableRefine[i].test(static_cast<unsigned>(tileIndex))) {
			ret[i] = 1;
		}
	}
	return ret;
}

template <typename TZone>
KOKKOS_INLINE_FUNCTION
typename TZone::RegionType
getSubZoneRegion(const TZone& zone, IdType subZoneLocalId,
	const SubdivisionInfo<TZone::dimension()>& subdivInfo)
{
	using RegionType = typename TZone::RegionType;
	using ScalarType = typename RegionType::ScalarType;
	using IntervalType = typename RegionType::IntervalType;

	constexpr auto subpavingDim = TZone::dimension();

	MultiIndex<subpavingDim> mId = subdivInfo.getMultiIndex(subZoneLocalId);

	const auto& zoneRegion = zone.getRegion();
	RegionType ret;
	for (auto i : makeIntervalRange(subpavingDim)) {
		const auto& ival = zoneRegion[i];
		auto delta = ival.length() / subdivInfo.getRatio()[i];
		ret[i] =
			IntervalType{ival.begin() + static_cast<ScalarType>(mId[i] * delta),
				ival.begin() + static_cast<ScalarType>((mId[i] + 1) * delta)};
	}
	return ret;
}

template <typename TData>
KOKKOS_INLINE_FUNCTION
IdType
countSelectSubZones(
	const TData& data, IdType index, const typename TData::ZoneType& zone)
{
	auto info = SubdivisionInfo<TData::subpavingDim>{
		getSubdivisionRatio(data, zone.getLevel(), index)};
	auto numSubRegions = info.getRatio().getProduct();
	auto selected = Kokkos::subview(data.selectedSubZones, index, Kokkos::ALL);
	IdType count = 0;
	for (auto i : makeIntervalRange(numSubRegions)) {
		auto subRegion = getSubZoneRegion(zone, i, info);
		if (data.detector(data.detector.selectTag, subRegion)) {
			selected(count) = i;
			++count;
		}
	}
	return count;
}

template <typename TData>
KOKKOS_INLINE_FUNCTION
void
countNewItemsFromTile(
	const TData& data, IdType index, ItemTotals& runningTotals)
{
	using RegionType = typename TData::ZoneType::RegionType;
	using BoolVec = refine::BoolVec<RegionType>;
	const auto& tile = data.tiles(index);
	auto zoneId = tile.getOwningZoneIndex();
	IdType count = 0;
	auto& zone = data.zones(zoneId);
	auto level = zone.getLevel();
	if (level < data.targetDepth) {
		BoolVec enable{};
		if (data.detector(data.detector.refineTag, tile.getRegion(), enable)) {
			for (DimType i = 0; i < data.subpavingDim; ++i) {
				if (enable[i]) {
					data.enableRefine[i].set(static_cast<unsigned>(index));
				}
				else {
					data.enableRefine[i].reset(static_cast<unsigned>(index));
				}
			}
			count = countSelectSubZones(data, index, zone);
		}
	}
	data.newZoneCounts(index) = count;
	if (count > 0) {
		runningTotals.zones += count;
		runningTotals.tiles += count - 1;
	}
}

template <typename TData>
KOKKOS_INLINE_FUNCTION
void
refineTile(const TData& data, IdType index)
{
	using ZoneType = typename TData::ZoneType;
	using TileType = typename TData::TileType;

	auto newZones = data.newZoneCounts(index);
	if (newZones == 0) {
		return;
	}

	auto& tile = data.tiles(index);
	auto ownerZoneId = tile.getOwningZoneIndex();
	auto& ownerZone = data.zones(ownerZoneId);
	auto level = ownerZone.getLevel();
	auto newLevel = level + 1;
	auto info = SubdivisionInfo<data.subpavingDim>{
		getSubdivisionRatio(data, level, index)};

	// Create first new zone, replace current tile and associate
	auto subZoneBeginId = data.numZones + data.subZoneStarts(index);
	data.zones(subZoneBeginId) = ZoneType{
		getSubZoneRegion(ownerZone, data.selectedSubZones(index, 0), info),
		newLevel, ownerZoneId};
	data.zones(subZoneBeginId).setTileIndex(index);
	tile = TileType{data.zones(subZoneBeginId).getRegion(), subZoneBeginId};

	// Create and associate remaining zones and tiles
	auto tileBeginId = data.numTiles + data.newTileStarts(index);
	for (IdType i = 1; i < newZones; ++i) {
		auto zoneId = subZoneBeginId + i;
		auto tileId = tileBeginId + i - 1;
		data.zones(zoneId) = ZoneType{
			getSubZoneRegion(ownerZone, data.selectedSubZones(index, i), info),
			newLevel, ownerZoneId};
		data.zones(zoneId).setTileIndex(tileId);
		data.tiles(tileId) = TileType{data.zones(zoneId).getRegion(), zoneId};
	}

	ownerZone.removeTile();
	ownerZone.setSubZoneIndices({subZoneBeginId, subZoneBeginId + newZones});
}

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
	for (_data.currLevel = 0; _data.currLevel < _data.targetDepth;
		 ++_data.currLevel) {
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
	_subpaving._refinementDepth = _data.currLevel;
}

template <typename TSubpaving, typename TDetector>
void
Refiner<TSubpaving, TDetector>::countNewItems()
{
	auto numTiles = _data.tiles.size();
	_data.newZoneCounts =
		Kokkos::View<IdType*>(AllocNoInit{"New Zone Counts"}, numTiles);
	auto numSubZones =
		_data.subdivisionInfos.h_view(_data.currLevel).getRatio().getProduct();
	_data.selectedSubZones = Kokkos::View<IdType**>(
		AllocNoInit{"Selected Sub-Zones"}, numTiles, numSubZones);
	std::for_each(begin(_data.enableRefine), end(_data.enableRefine),
		[numTiles](auto&& bitset) {
			using Bitset = std::remove_reference_t<decltype(bitset)>;
			bitset = Bitset(static_cast<unsigned>(numTiles));
		});
	ItemTotals counts{};
	auto data = _data;
	Kokkos::parallel_reduce(
		"CountNewItemsFromTile", numTiles,
		KOKKOS_LAMBDA(IdType id, ItemTotals & running) {
			countNewItemsFromTile(data, id, running);
		},
		counts);
	Kokkos::fence();
	_data.newItemTotals = counts;
}

template <typename TSubpaving, typename TDetector>
void
Refiner<TSubpaving, TDetector>::findNewItemIndices()
{
	auto numTiles = _data.tiles.size();
	auto subZoneStarts =
		Kokkos::View<IdType*>(AllocNoInit{"SubZone Start Ids"}, numTiles);
	auto newTileStarts =
		Kokkos::View<IdType*>(AllocNoInit{"Tile Start Ids"}, numTiles);
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

	auto data = _data;
	Kokkos::parallel_for(
		"RefineTile", data.numTiles,
		KOKKOS_LAMBDA(IdType id) { refineTile(data, id); });
	Kokkos::fence();

	_data.numZones = _data.zones.size();
	_data.numTiles = _data.tiles.size();
}
} // namespace detail
} // namespace plsm
