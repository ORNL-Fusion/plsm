#pragma once

//plsm
#include <plsm/MultiIndex.h>
#include <plsm/Utility.h>
#include <plsm/refine/Detector.h>

namespace plsm
{
namespace detail
{
class ExclusiveScanFunctor
{
public:
    ExclusiveScanFunctor(Kokkos::View<std::size_t*> data)
        :
        _data(data)
    {
    }

    KOKKOS_INLINE_FUNCTION
    void
    operator()(std::size_t index, std::size_t& update, const bool finalPass)
        const
    {
        const auto temp = _data(index);
        if (finalPass) {
            _data(index) = update;
        }
        update += temp;
    }

    KOKKOS_INLINE_FUNCTION
    void
    init(std::size_t& update) const
    {
      update = 0;
    }

    KOKKOS_INLINE_FUNCTION
    void
    join(volatile std::size_t& update, std::size_t input) const
    {
      update += input;
    }

private:
    Kokkos::View<std::size_t*> _data;
};


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
#ifdef TEST_SELECTOR
        this->countSelectNewItemsFromTile(index, running);
#else
        this->countNewItemsFromTile(index, running);
#endif
    }
};


template <typename TRefiner>
class InitZoneStarts : public RefinerFunctor<TRefiner>
{
public:
    using RefinerFunctor<TRefiner>::RefinerFunctor;

    KOKKOS_INLINE_FUNCTION
    void
    operator()(std::size_t index) const
    {
        this->_subZoneStarts(index) = this->_newZoneCounts(index);
    }
};


template <typename TRefiner>
class InitTileStarts : public RefinerFunctor<TRefiner>
{
public:
    using RefinerFunctor<TRefiner>::RefinerFunctor;

    KOKKOS_INLINE_FUNCTION
    void
    operator()(std::size_t index) const
    {
        this->_newTileStarts(index) = (this->_newZoneCounts(index) == 0) ? 0 :
            this->_newZoneCounts(index) - 1;
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


template <typename TScalar, std::size_t Dim, typename TEnumIndex,
    typename TItemData, typename TDetector>
Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
::Refiner(SubpavingType& subpaving, TDetector detector)
    :
    _subpaving(subpaving),
    _zones(subpaving._zones.d_view),
    _tiles(subpaving._tiles.d_view),
    _subdivisionInfos(subpaving._subdivisionInfos),
    _detector(detector),
    _numTiles(_tiles.extent(0)),
    _numZones(_zones.extent(0))
    // FIXME: These should start empty, default construction seems to work
    // ,
    // _selectedSubZones("Selected Sub-Zones", 1),
    // _newZoneCounts("New Zone Counts", 1),
    // _subZoneStarts("SubZone Start Indices", 1),
    // _newTileStarts("Tile Start Indices", 1)
{
}


template <typename TScalar, std::size_t Dim, typename TEnumIndex,
    typename TItemData, typename TDetector>
Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
::~Refiner()
{
    _subpaving._zones.d_view = _zones;
    _subpaving._zones.modify_device();
    _subpaving._tiles.d_view = _tiles;
    _subpaving._tiles.modify_device();
}


template <typename TScalar, std::size_t Dim, typename TEnumIndex,
    typename TItemData, typename TDetector>
void
Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
::operator()()
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
}


template <typename TScalar, std::size_t Dim, typename TEnumIndex,
    typename TItemData, typename TDetector>
KOKKOS_INLINE_FUNCTION
std::size_t
Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
::countSelectSubZones(std::size_t index, const ZoneType& zone) const
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


template <typename TScalar, std::size_t Dim, typename TEnumIndex,
    typename TItemData, typename TDetector>
KOKKOS_FUNCTION
void
Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
::countSelectNewItemsFromTile(std::size_t index, NewItemTotals& runningTotals)
    const
{
    const auto& tile = _tiles(index);
    auto zoneId = tile.getOwningZoneIndex();
    std::size_t count = 0;
    auto& zone = _zones(zoneId);
    auto level = zone.getLevel();
    // if (level == _currLevel) {
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


template <typename TScalar, std::size_t Dim, typename TEnumIndex,
    typename TItemData, typename TDetector>
KOKKOS_FUNCTION
void
Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
::countNewItemsFromTile(std::size_t index, NewItemTotals& runningTotals) const
{
    const auto& tile = _tiles(index);
    auto zoneId = tile.getOwningZoneIndex();
    std::size_t count = 0;
    auto level = _zones(zoneId).getLevel();
    if (level == _currLevel) {
        if (_detector(DetectorType::refineTag, tile.getRegion())) {
            count = _subdivisionInfos.d_view(level).getRatio().getProduct();
        }
    }
    _newZoneCounts(index) = count;
    if (count > 0) {
        runningTotals.zones += count;
        runningTotals.tiles += count - 1;
    }
}


template <typename TScalar, std::size_t Dim, typename TEnumIndex,
    typename TItemData, typename TDetector>
void
Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
::countNewZonesAndTiles()
{
    Kokkos::resize(_newZoneCounts, _numTiles);
    auto numSubZones = _subdivisionInfos.h_view(_currLevel).getRatio().getProduct();
    Kokkos::resize(_selectedSubZones, _numTiles, numSubZones);
    NewItemTotals counts{};
    Kokkos::parallel_reduce(_numTiles,
        CountNewItemsFromTile<Refiner>{*this},
        counts);
    _newItemTotals = counts;
}


template <typename TScalar, std::size_t Dim, typename TEnumIndex,
    typename TItemData, typename TDetector>
void
Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
::findNewItemIndices()
{
    Kokkos::resize(_subZoneStarts, _numTiles);
    Kokkos::parallel_for(_numTiles, InitZoneStarts<Refiner>{*this});
    Kokkos::parallel_scan(_numTiles, ExclusiveScanFunctor{_subZoneStarts});

    Kokkos::resize(_newTileStarts, _numTiles);
    Kokkos::parallel_for(_numTiles, InitTileStarts<Refiner>{*this});
    Kokkos::parallel_scan(_numTiles, ExclusiveScanFunctor{_newTileStarts});
}


template <typename TScalar, std::size_t Dim, typename TEnumIndex,
    typename TItemData, typename TDetector>
KOKKOS_INLINE_FUNCTION
typename
    Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
        ::RegionType
Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
::getSubZoneRegion(const ZoneType& zone, std::size_t subZoneLocalId) const
{
    using IntervalType = typename RegionType::IntervalType;

    const auto& info = _subdivisionInfos.d_view(zone.getLevel());
    MultiIndex<Dim> mId = info.getMultiIndex(subZoneLocalId);

    const auto& zoneRegion = zone.getRegion();
    RegionType ret;
    for (auto i : makeIntervalRange(Dim)) {
        const auto& ival = zoneRegion[i];
        auto delta = ival.length() / info.getRatio()[i];
        ret[i] = IntervalType{
            ival.begin() + static_cast<ScalarType>(mId[i]*delta),
            ival.begin() + static_cast<ScalarType>((mId[i]+1)*delta)};
    }
    return ret;
}


template <typename TScalar, std::size_t Dim, typename TEnumIndex,
    typename TItemData, typename TDetector>
KOKKOS_FUNCTION
void
Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
::refineTile(std::size_t index) const
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
        ZoneType{getSubZoneRegion(ownerZone, getSubZoneId(index, 0)), level,
            ownerZoneId};
    _zones(subZoneBeginId).setTileIndex(index);
    tile = TileType{_zones(subZoneBeginId).getRegion(), subZoneBeginId};

    //Create and associate remaining zones and tiles
    auto tileBeginId = _numTiles + _newTileStarts(index);
    for (std::size_t i = 1; i < newZones; ++i) {
        auto zoneId = subZoneBeginId + i;
        auto tileId = tileBeginId + i - 1;
        _zones(zoneId) =
            ZoneType{getSubZoneRegion(ownerZone, getSubZoneId(index, i)), level,
                ownerZoneId};
        _zones(zoneId).setTileIndex(tileId);
        _tiles(tileId) = TileType{_zones(zoneId).getRegion(), zoneId};
    }

    ownerZone.removeTile();
    ownerZone.setSubZoneIndices({subZoneBeginId, subZoneBeginId + newZones});
}


template <typename TScalar, std::size_t Dim, typename TEnumIndex,
    typename TItemData, typename TDetector>
void
Refiner<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>, TDetector>
::assignNewZonesAndTiles()
{
    Kokkos::resize(_zones, _zones.extent(0) + _newItemTotals.zones);
    Kokkos::resize(_tiles, _tiles.extent(0) + _newItemTotals.tiles);

    Kokkos::parallel_for(_numTiles, RefineTile<Refiner>{*this});

    _numTiles = _tiles.extent(0);
    _numZones = _zones.extent(0);
}
}
}
