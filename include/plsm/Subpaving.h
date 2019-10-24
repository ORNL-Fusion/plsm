#pragma once

//std
#include <vector>
#include <type_traits>
//Kokkos
#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>
//plsm
#include <plsm/detail/EnumIndexed.h>
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
    using RegionType = detail::EnumIndexed<Region<ScalarType, Dim>, TEnumIndex>;
    using PointType =
        detail::EnumIndexed<SpaceVector<ScalarType, Dim>, TEnumIndex>;
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

    Subpaving() = delete;

    Subpaving(const RegionType& region,
        const std::vector<SubdivisionRatioType>& subdivisionRatios);

    Subpaving(const Subpaving&) = delete;

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
    std::size_t
    getNumberOfTiles(TContext context = onHost)
    {
        return getTiles(context).extent(0);
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
    TilesView<TContext>&
    getTiles(TContext context = onHost)
    {
        return detail::getContextualView(_tiles, context);
    }

    template <typename TContext = OnHost>
    ZonesView<TContext>&
    getZones(TContext context = onHost)
    {
        return detail::getContextualView(_zones, context);
    }

    template <typename TContext = OnHost>
    std::size_t
    findTileId(const PointType& point, TContext context = onHost);

    void
    plot();

private:
    void
    processSubdivisionRatios(const std::vector<SubdivisionRatioType>&);

    template <typename TContext>
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
