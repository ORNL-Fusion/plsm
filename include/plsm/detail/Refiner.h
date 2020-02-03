#pragma once

#include <vector>
#include <utility>

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
    template <typename, std::size_t, typename, typename>
    friend class ::plsm::Subpaving;

    Refiner(SubpavingType& subpaving, DetectorType detector);

protected:
    SubpavingType& _subpaving;
    using ZonesView = typename SubpavingType::template ZonesView<OnDevice>;
    ZonesView _zones;
    using TilesView = typename SubpavingType::template TilesView<OnDevice>;
    TilesView _tiles;
    using SubdivisionInfoType = SubdivisionInfo<SubpavingType::dimension()>;
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
}
}

#include <plsm/detail/Refiner.inl>
