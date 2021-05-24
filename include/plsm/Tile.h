#pragma once

#include <type_traits>
#include <utility>

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
template <typename TRegion, typename TItemData = IdType>
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
	Tile() = default;

	/*!
	 * @brief Construct with Region and owning Zone index
	 */
	KOKKOS_INLINE_FUNCTION
	Tile(const RegionType& region, IdType owningZoneId) :
		_region(region), _owningZoneId(owningZoneId)
	{
	}

	//!@{
	/*!
	 * @brief Get/Set the Region mapped from by this Tile
	 */
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
		return _owningZoneId != invalid<IdType>;
	}
	//!}

	//!@{
	/*!
	 * @brief Get/Set index of owning Zone
	 */
	KOKKOS_INLINE_FUNCTION
	IdType
	getOwningZoneIndex() const noexcept
	{
		return _owningZoneId;
	}

	KOKKOS_INLINE_FUNCTION
	void
	setOwningZoneIndex(IdType id) noexcept
	{
		_owningZoneId = id;
	}
	//!@}

	/*!
	 * @brief Check if the Tile has mapped data
	 */
	KOKKOS_INLINE_FUNCTION
	bool
	hasData() const noexcept
	{
		return _data != invalid<ItemDataType>;
	}

	//!@{
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
	//!@}

private:
	//! Region mapped from by this Tile
	RegionType _region;
	//! Index of owning Zone
	IdType _owningZoneId{invalid<IdType>};

	// FIXME: Idea would be to use optional<ItemDataType> to hold arbitrary
	// data in a Tile. It needs to be initialized to an invalid state.
	static_assert(std::is_same<ItemDataType, IdType>::value,
		"Only IdType supported for now");
	//! Mapped user data item
	ItemDataType _data{invalid<ItemDataType>};
};
} // namespace plsm
