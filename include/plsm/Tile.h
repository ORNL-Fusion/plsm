#pragma once

//std
#include <type_traits>
#include <utility>
//plsm
#include <plsm/Region.h>
#include <plsm/Utility.h>

namespace plsm
{
template <typename TScalar, std::size_t Dim, typename TItemData = std::size_t>
class Tile
{
public:
    using RegionType = Region<TScalar, Dim>;
    using ItemDataType = TItemData;

    KOKKOS_INLINE_FUNCTION
    Tile() = default;

    KOKKOS_INLINE_FUNCTION
    Tile(const RegionType& region, std::size_t owningZoneId)
        :
        _region(region),
        _owningZoneId(owningZoneId)
    {
    }

    KOKKOS_INLINE_FUNCTION
    const RegionType&
    getRegion() const noexcept
    {
        return _region;
    }

    KOKKOS_INLINE_FUNCTION
    bool
    hasOwningZone() const noexcept
    {
        return _owningZoneId != invalid<std::size_t>;
    }

    KOKKOS_INLINE_FUNCTION
    std::size_t
    getOwningZoneIndex() const noexcept
    {
        return _owningZoneId;
    }

    KOKKOS_INLINE_FUNCTION
    void
    setOwningZoneIndex(std::size_t id) noexcept
    {
        _owningZoneId = id;
    }

    KOKKOS_INLINE_FUNCTION
    bool
    hasData() const noexcept
    {
        return _data != invalid<ItemDataType>;
    }

    KOKKOS_INLINE_FUNCTION
    ItemDataType&
    getData() noexcept
    {
        return _data;
    }

    KOKKOS_INLINE_FUNCTION
    const ItemDataType&
    getData() const noexcept
    {
        return _data;
    }

    KOKKOS_INLINE_FUNCTION
    void
    setData(const ItemDataType& data)
    {
        _data = data;
    }

    KOKKOS_INLINE_FUNCTION
    void
    setData(ItemDataType&& data)
    {
        _data = std::move(data);
    }

private:
    RegionType _region;
    std::size_t _owningZoneId{invalid<std::size_t>};

    // FIXME: Idea would be to use optional<ItemDataType> to hold arbitrary
    // data in a Tile. It needs to be initialized to an invalid state.
    static_assert(std::is_same<ItemDataType, std::size_t>::value,
        "Only std::size_t supported for now");
    ItemDataType _data{invalid<ItemDataType>};
};
}
