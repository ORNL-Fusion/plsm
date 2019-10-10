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
    using ZoneType = Zone<RegionType>;
    using ZonesDualView = Kokkos::DualView<ZoneType*>;
    using ZonesView = typename ZonesDualView::t_dev;
    using ZonesHostView = typename ZonesDualView::t_host;
    using SubdivisionRatioType = SubdivisionRatio<Dim>;
    using SubdivisionInfoType = SubdivisionInfo<Dim>;
    using ItemDataType = TItemData;
    using TileType = Tile<RegionType, ItemDataType>;
    using TilesDualView = Kokkos::DualView<TileType*>;
    using TilesView = typename TilesDualView::t_dev;
    using TilesHostView = typename TilesDualView::t_host;

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

    std::size_t
    getNumberOfTiles()
    {
        return getTilesOnHost().extent(0);
    }

    TilesHostView
    getTilesOnHost();

    ZonesHostView
    getZonesOnHost();

    std::size_t
    getTileId(const PointType& point);

    void
    plot();

private:
    void
    processSubdivisionRatios(const std::vector<SubdivisionRatioType>&);

    std::size_t
    getTileId(const PointType& point, const ZoneType& zone) const;

private:
    ZonesDualView _zones;
    TilesDualView _tiles;
    RegionType _rootRegion;
    Kokkos::DualView<SubdivisionInfoType*> _subdivisionInfos;
    std::size_t _refinementDepth{};
};
}

#include <plsm/Subpaving.inl>
