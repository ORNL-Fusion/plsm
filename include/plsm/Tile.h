#pragma once

//std
#include <type_traits>
#include <utility>
//plsm
#include <plsm/Region.h>
#include <plsm/Utility.h>

namespace plsm
{
/*!
 * @brief Tile is used as a non-overlapped lattice region
 *
 * Tile expects to be used in a Subpaving and to "owned" by an existing Zone in
 * that Subpaving.
 *
 * @tparam TRegion Type used for lattice region
 * @tparam TItemData User data type to be mapped from Tile
 *
 * @test test_Tile.cpp
 */
template <typename TRegion, typename TItemData = std::size_t>
class Tile
{
public:
    //! Alias for Region
    using RegionType = TRegion;
    //! User data type to be mapped from Tile
    using ItemDataType = TItemData;

    /*!
     * @brief Default construct with empty region, no owner, and no data item
     */
    KOKKOS_INLINE_FUNCTION
    Tile() = default;

    /*!
     * @brief Construct with Region and owning Zone index
     */
    KOKKOS_INLINE_FUNCTION
    Tile(const RegionType& region, std::size_t owningZoneId)
        :
        _region(region),
        _owningZoneId(owningZoneId)
    {
    }

    /*!
     * @brief Get the Region mapped from by this Tile
     */
    KOKKOS_INLINE_FUNCTION
    const RegionType&
    getRegion() const noexcept
    {
        return _region;
    }

    /*!
     * @brief Check if the Tile has a valid owner index
     */
    KOKKOS_INLINE_FUNCTION
    bool
    hasOwningZone() const noexcept
    {
        return _owningZoneId != invalid<std::size_t>;
    }

    //@{
    /*!
     * @brief Get/Set index of owning Zone
     */
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
    //@}

    /*!
     * @brief Check if the Tile has mapped data
     */
    KOKKOS_INLINE_FUNCTION
    bool
    hasData() const noexcept
    {
        return _data != invalid<ItemDataType>;
    }

    //@{
    /*!
     * @brief Get/Set mapped data item
     */
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
    //@}

private:
    //! Region mapped from by this Tile
    RegionType _region;
    //! Index of owning Zone
    std::size_t _owningZoneId{invalid<std::size_t>};

    // FIXME: Idea would be to use optional<ItemDataType> to hold arbitrary
    // data in a Tile. It needs to be initialized to an invalid state.
    static_assert(std::is_same<ItemDataType, std::size_t>::value,
        "Only std::size_t supported for now");
    //! Mapped user data item
    ItemDataType _data{invalid<ItemDataType>};
};
}
