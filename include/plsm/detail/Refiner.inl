#pragma once

#include <plsm/MultiIndex.h>
#include <plsm/Utility.h>
#include <plsm/refine/Detector.h>

namespace plsm
{
namespace detail
{
template <typename TRefiner>
class RefinerFunctor : public TRefiner
{
public:
    RefinerFunctor(const TRefiner& refiner)
        :
        TRefiner(refiner)
    {
    }
};


template <typename TRefiner>
class CountNewItemsFromTile : public RefinerFunctor<TRefiner>
{
public:
    using NewItemTotals = typename TRefiner::NewItemTotals;

    using RefinerFunctor<TRefiner>::RefinerFunctor;

    KOKKOS_INLINE_FUNCTION
    void
    operator()(std::size_t index, NewItemTotals& running) const
    {
        this->countSelectNewItemsFromTile(index, running);
    }
};


template <typename TRefiner>
class RefineTile : public RefinerFunctor<TRefiner>
{
public:
    using RefinerFunctor<TRefiner>::RefinerFunctor;

    KOKKOS_INLINE_FUNCTION
    void
    operator()(std::size_t index) const
    {
        this->refineTile(index);
    }
};


template <typename TSubpaving, typename TDetector>
Refiner<TSubpaving, TDetector>::Refiner(SubpavingType& subpaving,
        TDetector detector)
    :
    _subpaving(subpaving),
    _zones(subpaving._zones.d_view),
    _tiles(subpaving._tiles.d_view),
    _subdivisionInfos(subpaving._subdivisionInfos),
    _detector(detector),
    _numTiles(_tiles.extent(0)),
    _numZones(_zones.extent(0))
{
}


template <typename TSubpaving, typename TDetector>
void
Refiner<TSubpaving, TDetector>::operator()()
{
    _targetDepth = (_detector.depth() == _detector.fullDepth) ?
        this->_subdivisionInfos.h_view.extent(0) : _detector.depth();

    for (_currLevel = 0; _currLevel < _targetDepth; ++_currLevel) {
        countNewZonesAndTiles();

        if (_newItemTotals.zones == 0) {
            break;
        }

        findNewItemIndices();

        assignNewZonesAndTiles();
    }

    _subpaving._zones.d_view = _zones;
    _subpaving._zones.modify_device();
    _subpaving._tiles.d_view = _tiles;
    _subpaving._tiles.modify_device();
}


template <typename TSubpaving, typename TDetector>
KOKKOS_INLINE_FUNCTION
std::size_t
Refiner<TSubpaving, TDetector>::countSelectSubZones(std::size_t index,
    const ZoneType& zone) const
{
    const auto& info = _subdivisionInfos.d_view(zone.getLevel());
    auto numSubRegions = info.getRatio().getProduct();
    auto selected = Kokkos::subview(_selectedSubZones, index, Kokkos::ALL);
    std::size_t count = 0;
    for (auto i : makeIntervalRange(numSubRegions)) {
        auto subRegion = getSubZoneRegion(zone, i);
        if (_detector(DetectorType::selectTag, subRegion)) {
            selected(count) = i;
            ++count;
        }
    }
    return count;
}


template <typename TSubpaving, typename TDetector>
KOKKOS_FUNCTION
void
Refiner<TSubpaving, TDetector>::countSelectNewItemsFromTile(std::size_t index,
    NewItemTotals& runningTotals) const
{
    const auto& tile = _tiles(index);
    auto zoneId = tile.getOwningZoneIndex();
    std::size_t count = 0;
    auto& zone = _zones(zoneId);
    auto level = zone.getLevel();
    if (level < _targetDepth) {
        if (_detector(DetectorType::refineTag, tile.getRegion())) {
            count = countSelectSubZones(index, zone);
        }
    }
    _newZoneCounts(index) = count;
    if (count > 0) {
        runningTotals.zones += count;
        runningTotals.tiles += count - 1;
    }
}


template <typename TSubpaving, typename TDetector>
void
Refiner<TSubpaving, TDetector>::countNewZonesAndTiles()
{
    _newZoneCounts = Kokkos::View<std::size_t*>(
        Kokkos::ViewAllocateWithoutInitializing{"New Zone Counts"}, _numTiles);
    auto numSubZones =
        _subdivisionInfos.h_view(_currLevel).getRatio().getProduct();
    _selectedSubZones = Kokkos::View<std::size_t**>(
        Kokkos::ViewAllocateWithoutInitializing{"Selected Sub-Zones"},
        _numTiles, numSubZones);
    NewItemTotals counts{};
    Kokkos::parallel_reduce(_numTiles, CountNewItemsFromTile<Refiner>{*this},
        counts);
    Kokkos::fence();
    _newItemTotals = counts;
}


template <typename TSubpaving, typename TDetector>
void
Refiner<TSubpaving, TDetector>::findNewItemIndices()
{
    auto subZoneStarts = Kokkos::View<std::size_t*>(
        Kokkos::ViewAllocateWithoutInitializing{"SubZone Start Ids"},
        _numTiles);
    auto newTileStarts = Kokkos::View<std::size_t*>(
        Kokkos::ViewAllocateWithoutInitializing{"Tile Start Ids"}, _numTiles);
    auto newZoneCounts = _newZoneCounts;

    //Initialize starts
    Kokkos::parallel_for(_numTiles, KOKKOS_LAMBDA (std::size_t i) {
        auto newZoneCount = newZoneCounts(i);
        subZoneStarts(i) = newZoneCount;
        newTileStarts(i) = (newZoneCount == 0) ? 0 : newZoneCount - 1;
    });

    //Scan zone starts
    Kokkos::parallel_scan(_numTiles, KOKKOS_LAMBDA (std::size_t i,
            std::size_t& update, const bool finalPass) {
        const auto tmp = subZoneStarts(i);
        if (finalPass) {
            subZoneStarts(i) = update;
        }
        update += tmp;
    });

    //Scan tile starts
    Kokkos::parallel_scan(_numTiles, KOKKOS_LAMBDA (std::size_t i,
            std::size_t& update, const bool finalPass) {
        const auto tmp = newTileStarts(i);
        if (finalPass) {
            newTileStarts(i) = update;
        }
        update += tmp;
    });

    Kokkos::fence();
    _subZoneStarts = subZoneStarts;
    _newTileStarts = newTileStarts;
}


template <typename TSubpaving, typename TDetector>
KOKKOS_INLINE_FUNCTION
typename Refiner<TSubpaving, TDetector>::RegionType
Refiner<TSubpaving, TDetector>::getSubZoneRegion(const ZoneType& zone,
    std::size_t subZoneLocalId) const
{
    using IntervalType = typename RegionType::IntervalType;

    constexpr auto dim = SubpavingType::dimension();
    const auto& info = _subdivisionInfos.d_view(zone.getLevel());
    MultiIndex<dim> mId = info.getMultiIndex(subZoneLocalId);

    const auto& zoneRegion = zone.getRegion();
    RegionType ret;
    for (auto i : makeIntervalRange(dim)) {
        const auto& ival = zoneRegion[i];
        auto delta = ival.length() / info.getRatio()[i];
        ret[i] = IntervalType{
            ival.begin() + static_cast<ScalarType>(mId[i]*delta),
            ival.begin() + static_cast<ScalarType>((mId[i]+1)*delta)};
    }
    return ret;
}


template <typename TSubpaving, typename TDetector>
KOKKOS_FUNCTION
void
Refiner<TSubpaving, TDetector>::refineTile(std::size_t index) const
{
    auto newZones = _newZoneCounts(index);
    if (newZones == 0) {
        return;
    }

    auto& tile = _tiles(index);
    auto ownerZoneId = tile.getOwningZoneIndex();
    auto& ownerZone = _zones(ownerZoneId);
    auto level = ownerZone.getLevel() + 1;

    //Create first new zone, replace current tile and associate
    auto subZoneBeginId = _numZones + _subZoneStarts(index);
    _zones(subZoneBeginId) =
        ZoneType{getSubZoneRegion(ownerZone, _selectedSubZones(index, 0)),
            level, ownerZoneId};
    _zones(subZoneBeginId).setTileIndex(index);
    tile = TileType{_zones(subZoneBeginId).getRegion(), subZoneBeginId};

    //Create and associate remaining zones and tiles
    auto tileBeginId = _numTiles + _newTileStarts(index);
    for (std::size_t i = 1; i < newZones; ++i) {
        auto zoneId = subZoneBeginId + i;
        auto tileId = tileBeginId + i - 1;
        _zones(zoneId) =
            ZoneType{getSubZoneRegion(ownerZone, _selectedSubZones(index, i)),
                level, ownerZoneId};
        _zones(zoneId).setTileIndex(tileId);
        _tiles(tileId) = TileType{_zones(zoneId).getRegion(), zoneId};
    }

    ownerZone.removeTile();
    ownerZone.setSubZoneIndices({subZoneBeginId, subZoneBeginId + newZones});
}


template <typename TSubpaving, typename TDetector>
void
Refiner<TSubpaving, TDetector>::assignNewZonesAndTiles()
{
    Kokkos::resize(_zones, _zones.extent(0) + _newItemTotals.zones);
    Kokkos::resize(_tiles, _tiles.extent(0) + _newItemTotals.tiles);

    Kokkos::parallel_for(_numTiles, RefineTile<Refiner>{*this});
    Kokkos::fence();

    _numTiles = _tiles.extent(0);
    _numZones = _zones.extent(0);
}
}
}
