#pragma once

//plsm
#include <plsm/IntervalRange.h>
#include <plsm/Region.h>
#include <plsm/Tile.h>
#include <plsm/Utility.h>

namespace plsm
{
template <typename TScalar, std::size_t Dim>
struct Zone
{
public:
    using RegionType = Region<TScalar, Dim>;

    KOKKOS_INLINE_FUNCTION
    Zone() = default;

    KOKKOS_INLINE_FUNCTION
    Zone(const RegionType& region, std::size_t level,
            std::size_t parentId = invalid<std::size_t>)
        :
        _region{region},
        _level{level},
        _parentId{parentId}
    {
    }

    KOKKOS_INLINE_FUNCTION
    bool
    hasTile() const noexcept
    {
        return (_tileId != invalid<std::size_t>);
    }

    KOKKOS_INLINE_FUNCTION
    std::size_t
    getTileIndex() const noexcept
    {
        return _tileId;
    }

    KOKKOS_INLINE_FUNCTION
    void
    setTileIndex(std::size_t tileId) noexcept
    {
        _tileId = tileId;
    }

    KOKKOS_INLINE_FUNCTION
    void
    removeTile() noexcept
    {
        _tileId = invalid<std::size_t>;
    }

    KOKKOS_INLINE_FUNCTION
    std::size_t
    getLevel() const noexcept
    {
        return _level;
    }

    KOKKOS_INLINE_FUNCTION
    const Interval<std::size_t>&
    getSubZoneIndices() const noexcept
    {
        return _subZoneIds;
    }

    KOKKOS_INLINE_FUNCTION
    IntervalRange<std::size_t>
    getSubZoneRange() const noexcept
    {
        return IntervalRange<std::size_t>{_subZoneIds};
    }

    KOKKOS_INLINE_FUNCTION
    void
    setSubZoneIndices(const Interval<std::size_t>& subZoneIds)
    {
        _subZoneIds = subZoneIds;
    }

    KOKKOS_INLINE_FUNCTION
    const RegionType&
    getRegion() const noexcept
    {
        return _region;
    }

    KOKKOS_INLINE_FUNCTION
    bool
    hasParent() const noexcept
    {
        return (_parentId != invalid<std::size_t>);
    }

    KOKKOS_INLINE_FUNCTION
    std::size_t
    getParentIndex() const noexcept
    {
        return _parentId;
    }

private:
    RegionType _region;
    std::size_t _level{};
    std::size_t _parentId{invalid<std::size_t>};
    Interval<std::size_t> _subZoneIds;
    std::size_t _tileId{invalid<std::size_t>};
};
}
