#pragma once

//std
#include <vector>
#include <utility>
//Kokkos
#include <Kokkos_Vector.hpp>
//plsm
#include <plsm/SpaceVector.h>
#include <plsm/Subpaving.h>
#include <plsm/Utility.h>

namespace plsm
{
template <typename TScalar, std::size_t Dim, typename TItemData>
class Subpaving;


namespace detail
{
template <typename TSubpaving, typename TDetector>
class Refiner
{
};


template <typename TScalar, std::size_t Dim, typename TItemData,
    typename TDetector>
class Refiner<::plsm::Subpaving<TScalar, Dim, TItemData>, TDetector>
{
public:
    using SubpavingType = ::plsm::Subpaving<TScalar, Dim, TItemData>;
    using ScalarType = typename SubpavingType::ScalarType;
    using ZoneType = typename SubpavingType::ZoneType;
    using TileType = typename SubpavingType::TileType;
    using RegionType = typename SubpavingType::RegionType;
    using DetectorType = TDetector;

    Refiner(SubpavingType& subpaving, DetectorType detector);

    ~Refiner();

    void
    operator()();

    struct NewItemTotals
    {
        std::size_t zones = 0;
        std::size_t tiles = 0;

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
    std::size_t
    countSelectSubZones(std::size_t index, const ZoneType& zone) const;

    KOKKOS_FUNCTION
    void
    countSelectNewItemsFromTile(std::size_t index, NewItemTotals& runningTotals)
        const;

    KOKKOS_FUNCTION
    void
    countNewItemsFromTile(std::size_t index, NewItemTotals& runningTotals)
        const;

    void
    countNewZonesAndTiles();

    void
    findNewItemIndices();

    KOKKOS_FUNCTION
    void
    refineTile(std::size_t index) const;

    void
    assignNewZonesAndTiles();

    KOKKOS_INLINE_FUNCTION
    RegionType
    getSubZoneRegion(const ZoneType& zone, std::size_t subZoneLocalId) const;

    KOKKOS_INLINE_FUNCTION
    std::size_t
    getSubZoneId(std::size_t tileId, std::size_t subZoneLocalId) const
    {
#ifdef TEST_SELECTOR
        return _selectedSubZones(tileId, subZoneLocalId);
#else
        return subZoneLocalId;
#endif
    }

protected:
    SubpavingType& _subpaving;
    using ZonesView = typename SubpavingType::ZonesView;
    ZonesView _zones;
    using TilesView = typename SubpavingType::TilesView;
    TilesView _tiles;
    using SubdivisionInfoType = typename SubpavingType::SubdivisionInfoType;
    Kokkos::DualView<SubdivisionInfoType*> _subdivisionInfos;

    std::size_t _currLevel;
    std::size_t _targetDepth;

    DetectorType _detector;

    Kokkos::View<std::size_t**> _selectedSubZones;

    NewItemTotals _newItemTotals{};
    std::size_t _numTiles;
    std::size_t _numZones;
    Kokkos::View<std::size_t*> _newZoneCounts;
    Kokkos::View<std::size_t*> _subZoneStarts;
    Kokkos::View<std::size_t*> _newTileStarts;
};


template <typename TSubpaving, typename TDetector>
Refiner<TSubpaving, TDetector>
makeRefiner(TSubpaving& subpaving, TDetector&& detector)
{
    return Refiner<TSubpaving, TDetector>{subpaving,
        std::forward<TDetector>(detector)};
}
}
}

#include <plsm/detail/Refiner.inl>
