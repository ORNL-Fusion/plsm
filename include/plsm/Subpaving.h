#pragma once

#include <vector>
#include <type_traits>

#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>

#include <plsm/ContextUtility.h>
#include <plsm/EnumIndexed.h>
#include <plsm/Zone.h>
#include <plsm/detail/Refiner.h>
#include <plsm/detail/SubdivisionInfo.h>

namespace plsm
{
template <typename TScalar, std::size_t Dim, typename TEnumIndex = void,
    typename TItemData = std::size_t>
class Subpaving
{
    template <typename TSubpaving, typename TSelector>
    friend class detail::Refiner;

public:
    using ScalarType = TScalar;
    using RegionType = EnumIndexed<Region<ScalarType, Dim>, TEnumIndex>;
    using PointType = EnumIndexed<SpaceVector<ScalarType, Dim>, TEnumIndex>;
    using IntervalType = typename RegionType::IntervalType;
    using SubdivisionRatioType = SubdivisionRatio<Dim>;
    using ItemDataType = TItemData;

    using ZoneType = Zone<RegionType>;
    using ZonesDualView = Kokkos::DualView<ZoneType*>;

    template <typename TContext>
    using ZonesView = detail::ContextualViewType<ZonesDualView, TContext>;

    using TileType = Tile<RegionType, ItemDataType>;
    using TilesDualView = Kokkos::DualView<TileType*>;

    template <typename TContext>
    using TilesView = detail::ContextualViewType<TilesDualView, TContext>;

    template <typename T>
    using Array = Kokkos::Array<T, Dim>;

    Subpaving() = delete;

    Subpaving(const RegionType& region,
        const std::vector<SubdivisionRatioType>& subdivisionRatios);

    Subpaving(const Subpaving&) = default;
    Subpaving& operator=(const Subpaving&) = default;

    static
    KOKKOS_INLINE_FUNCTION
    constexpr std::size_t
    dimension() noexcept
    {
        return Dim;
    }

    template <typename TRefinementDetector>
    void
    refine(TRefinementDetector&& detector);

    const RegionType&
    getLatticeRegion() const noexcept
    {
        return _rootRegion;
    }

    TilesDualView
    getTilesView()
    {
        return _tiles;
    }

    template <typename TContext = OnHost>
    void
    syncTiles(TContext context = onHost)
    {
        detail::syncUpdate(_tiles, context);
    }

    template <typename TContext = OnHost>
    void
    syncZones(TContext context = onHost)
    {
        detail::syncUpdate(_zones, context);
    }

    template <typename TContext = OnHost>
    void
    syncAll(TContext context = onHost)
    {
        syncTiles(context);
        syncZones(context);
    }

    template <typename TContext = OnHost>
    std::size_t
    getNumberOfTiles(TContext context = onHost)
    {
        return getTiles(context).extent(0);
    }

    template <typename TContext = OnHost>
    KOKKOS_INLINE_FUNCTION
    const TilesView<TContext>&
    getTiles(TContext context = onHost) const
    {
        return detail::getContextualView(_tiles, context);
    }

    template <typename TContext = OnHost>
    KOKKOS_INLINE_FUNCTION
    const ZonesView<TContext>&
    getZones(TContext context = onHost) const
    {
        return detail::getContextualView(_zones, context);
    }

    template <typename TContext = OnHost>
    KOKKOS_INLINE_FUNCTION
    std::size_t
    findTileId(const PointType& point, TContext context = onHost) const;

    void
    plot();

private:
    void
    processSubdivisionRatios(const std::vector<SubdivisionRatioType>&);

private:
    ZonesDualView _zones;
    TilesDualView _tiles;
    RegionType _rootRegion;
    Kokkos::DualView<detail::SubdivisionInfo<Dim>*> _subdivisionInfos;
    std::size_t _refinementDepth{};
};
}

#include <plsm/Subpaving.inl>
