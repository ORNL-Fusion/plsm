#pragma once

#include <type_traits>
#include <utility>

#include <plsm/Region.h>
#include <plsm/Utility.h>

namespace plsm
{
template <typename TRegion, typename TItem = void>
struct TileData
{
	//! Alias for Region
	using RegionType = TRegion;
	//! User data type to be mapped from Tile
	using ItemType = TItem;

	KOKKOS_INLINE_FUNCTION
	decltype(auto)
	getItem() noexcept
	{
		return item;
	}

	KOKKOS_INLINE_FUNCTION
	decltype(auto)
	getItem() const noexcept
	{
		return item;
	}

	template <typename T>
	KOKKOS_INLINE_FUNCTION
	void
	setItem(T&& v)
	{
		item = std::forward<T>(v);
	}

	//! Region mapped from by this Tile
	RegionType region;
	//! Index of owning Zone
	IdType owningZoneId{invalid<IdType>};

	static_assert(std::is_integral_v<ItemType>);
	//! Mapped user data item
	ItemType item{invalid<ItemType>};
};

template <typename TRegion>
struct TileData<TRegion, void>
{
	//! Alias for Region
	using RegionType = TRegion;

	KOKKOS_INLINE_FUNCTION
	decltype(auto)
	getItem() noexcept
	{
		return invalid<IdType>;
	}

	KOKKOS_INLINE_FUNCTION
	decltype(auto)
	getItem() const noexcept
	{
		return invalid<IdType>;
	}

	template <typename T>
	KOKKOS_INLINE_FUNCTION
	void
	setItem([[maybe_unused]] T&&)
	{
	}

	//! Region mapped from by this Tile
	RegionType region;
	//! Index of owning Zone
	IdType owningZoneId{invalid<IdType>};
};

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
template <typename TRegion, typename TItemData = void>
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
		_data{region, owningZoneId}
	{
	}

	/*!
	 * @brief Get/Set the Region mapped from by this Tile
	 */
	KOKKOS_INLINE_FUNCTION
	const RegionType&
	getRegion() const noexcept
	{
		return _data.region;
	}

	/*!
	 * @brief Check if this tile has a valid owning zone id
	 */
	KOKKOS_INLINE_FUNCTION
	bool
	hasOwningZone() const noexcept
	{
		return _data.owningZoneId != invalid<IdType>;
	}

	//!@{
	/*!
	 * @brief Get/Set index of owning Zone
	 */
	KOKKOS_INLINE_FUNCTION
	IdType
	getOwningZoneIndex() const noexcept
	{
		return _data.owningZoneId;
	}

	KOKKOS_INLINE_FUNCTION
	void
	setOwningZoneIndex(IdType id) noexcept
	{
		_data.owningZoneId = id;
	}
	//!@}

	/*!
	 * @brief Check if the Tile has mapped data
	 */
	KOKKOS_INLINE_FUNCTION
	bool
	hasData() const noexcept
	{
		return !isInvalid(_data.getItem());
	}

	//!@{
	/*!
	 * @brief Get/Set mapped data item
	 */
	KOKKOS_INLINE_FUNCTION
	decltype(auto)
	getData() noexcept
	{
		return _data.getItem();
	}

	KOKKOS_INLINE_FUNCTION
	decltype(auto)
	getData() const noexcept
	{
		return _data.getItem();
	}

	template <typename T>
	KOKKOS_INLINE_FUNCTION
	void
	setData(T&& data)
	{
		_data.setItem(std::forward<T>(data));
	}
	//!@}

private:
	TileData<RegionType, ItemDataType> _data;
};
} // namespace plsm
