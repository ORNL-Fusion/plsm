#pragma once

#include <vector>
#include <type_traits>

#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>

#include <plsm/EnumIndexed.h>
#include <plsm/detail/Refiner.h>
#include <plsm/ContextUtility.h>
#include <plsm/MultiIndex.h>
#include <plsm/Zone.h>

namespace plsm
{
template <std::size_t Dim>
using SubdivisionRatio = MultiIndex<Dim>;


template <std::size_t Dim>
class SubdivisionInfo
{
public:
    KOKKOS_INLINE_FUNCTION
    SubdivisionInfo() noexcept
        = default;

    KOKKOS_INLINE_FUNCTION
    SubdivisionInfo(const SubdivisionRatio<Dim>& ratio)
        :
        _ratio{ratio}
    {
        _sliceSize[Dim-1] = 1;
        for (auto i : makeIntervalRange(Dim - 1)) {
            _sliceSize[Dim-2-i] = _sliceSize[Dim-1-i]*_ratio[Dim-1-i];
        }
    }

    KOKKOS_INLINE_FUNCTION
    const SubdivisionRatio<Dim>&
    getRatio() const noexcept
    {
        return _ratio;
    }

    KOKKOS_INLINE_FUNCTION
    std::size_t
    getLinearIndex(const MultiIndex<Dim>& mId) const
    {
        std::size_t ret = 0;
        for (auto i : makeIntervalRange(Dim)) {
            ret += mId[i]*_sliceSize[i];
        }
        return ret;
    }

    KOKKOS_INLINE_FUNCTION
    MultiIndex<Dim>
    getMultiIndex(std::size_t linearIndex) const
    {
        MultiIndex<Dim> ret;
        for (auto i : makeIntervalRange(Dim)) {
            ret[i] = linearIndex / _sliceSize[i];
            linearIndex = linearIndex % _sliceSize[i];
        }
        return ret;
    }

private:
    SubdivisionRatio<Dim> _ratio;
    MultiIndex<Dim> _sliceSize;
};


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
    using SubdivisionInfoType = SubdivisionInfo<Dim>;
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

    Subpaving(Subpaving&&) = default;
    Subpaving& operator=(Subpaving&&) = default;

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
    TilesView<TContext>&
    getTiles(TContext context = onHost)
    {
        return detail::getContextualView(_tiles, context);
    }

    template <typename TContext = OnHost>
    KOKKOS_INLINE_FUNCTION
    ZonesView<TContext>&
    getZones(TContext context = onHost)
    {
        return detail::getContextualView(_zones, context);
    }

    template <typename TContext = OnHost>
    KOKKOS_INLINE_FUNCTION
    std::size_t
    findTileId(const PointType& point, TContext context = onHost);

    void
    plot();

private:
    void
    processSubdivisionRatios(const std::vector<SubdivisionRatioType>&);

    template <typename TContext>
    KOKKOS_INLINE_FUNCTION
    std::size_t
    findTileId(const PointType& point, const ZoneType& zone, TContext context);

private:
    ZonesDualView _zones;
    TilesDualView _tiles;
    RegionType _rootRegion;
    Kokkos::DualView<SubdivisionInfoType*> _subdivisionInfos;
    std::size_t _refinementDepth{};
};
}

#include <plsm/Subpaving.inl>
