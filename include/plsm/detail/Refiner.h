#pragma once

#include <vector>
#include <utility>

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
    static constexpr std::size_t subpavingDim = SubpavingType::dimension();

public:
    using SubdivisionRatioType = ::plsm::SubdivisionRatio<subpavingDim>;
    using SubdivisionInfoType = SubdivisionInfo<subpavingDim>;

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

    KOKKOS_INLINE_FUNCTION
    void
    countSelectNewItemsFromTile(std::size_t index, NewItemTotals& runningTotals)
        const;

    void
    countNewZonesAndTiles();

    void
    findNewItemIndices();

    KOKKOS_INLINE_FUNCTION
    void
    refineTile(std::size_t index) const;

    void
    assignNewZonesAndTiles();

    KOKKOS_INLINE_FUNCTION
    RegionType
    getSubZoneRegion(const ZoneType& zone, std::size_t subZoneLocalId,
        const SubdivisionInfoType& subdivInfo) const;

    KOKKOS_INLINE_FUNCTION
    SubdivisionRatioType
    getSubdivisionRatio(std::size_t level, std::size_t tileIndex) const;

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

    Kokkos::DualView<SubdivisionInfoType*> _subdivisionInfos;

    std::size_t _currLevel;
    std::size_t _targetDepth;

    DetectorType _detector;

    using DefaultExecSpace =
        typename Kokkos::View<int*>::traits::execution_space;
    Kokkos::Array<Kokkos::Bitset<DefaultExecSpace>, subpavingDim> _enableRefine;

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
