#pragma once

#include <plsm/IntervalRange.h>
#include <plsm/Region.h>
#include <plsm/Tile.h>
#include <plsm/Utility.h>

namespace plsm
{
/*!
 * @brief Zone is used for recursively subdividing a lattice region
 *
 * Zone expects to be used in a Subpaving and have relationships with other
 * Zones in a hierarchical subdivision by means of indices (for parent and
 * children) and potentially an owning relationship with a Tile.
 *
 * @tparam TRegion Type used for lattice region
 *
 * @test test_Zone.cpp
 */
template <typename TRegion>
struct Zone
{
public:
	//! Alias for Region
	using RegionType = TRegion;

	/*!
	 * @brief Default construct with empty Region, no parent, no children, and
	 * no Tile at level 0
	 */
	Zone() = default;

	/*!
	 * @brief Construct with Region, subdivision level, and index or parent Zone
	 */
	KOKKOS_INLINE_FUNCTION
	Zone(const RegionType& region, std::size_t level,
		std::size_t parentId = invalid<std::size_t>) :
		_region{region}, _level{level}, _parentId{parentId}
	{
	}

	/*!
	 * @brief Check if Zone owns a Tile (has a valid Tile index)
	 */
	KOKKOS_INLINE_FUNCTION
	bool
	hasTile() const noexcept
	{
		return (_tileId != invalid<std::size_t>);
	}

	/*!
	 * @brief Get index of owned Tile
	 */
	KOKKOS_INLINE_FUNCTION
	std::size_t
	getTileIndex() const noexcept
	{
		return _tileId;
	}

	/*!
	 * @brief Set index of owned Tile
	 */
	KOKKOS_INLINE_FUNCTION
	void
	setTileIndex(std::size_t tileId) noexcept
	{
		_tileId = tileId;
	}

	/*!
	 * @brief Disown Tile (reset Tile index)
	 */
	KOKKOS_INLINE_FUNCTION
	void
	removeTile() noexcept
	{
		_tileId = invalid<std::size_t>;
	}

	/*!
	 * @brief Get subdivision level
	 */
	KOKKOS_INLINE_FUNCTION
	std::size_t
	getLevel() const noexcept
	{
		return _level;
	}

	/*!
	 * @brief Get Interval of indices to subzones
	 */
	KOKKOS_INLINE_FUNCTION
	const Interval<std::size_t>&
	getSubZoneIndices() const noexcept
	{
		return _subZoneIds;
	}

	/*!
	 * @brief Set Interval of indices to subzones
	 */
	KOKKOS_INLINE_FUNCTION
	void
	setSubZoneIndices(const Interval<std::size_t>& subZoneIds)
	{
		_subZoneIds = subZoneIds;
	}

	/*!
	 * @brief Get IntervalRange over subzone indices
	 */
	KOKKOS_INLINE_FUNCTION
	IntervalRange<std::size_t>
	getSubZoneRange() const noexcept
	{
		return IntervalRange<std::size_t>{_subZoneIds};
	}

	/*!
	 * @brief Get the Zone's Region
	 */
	KOKKOS_INLINE_FUNCTION
	const RegionType&
	getRegion() const noexcept
	{
		return _region;
	}

	/*!
	 * @brief Check if the Zone has a valid index to a parent Zone
	 */
	KOKKOS_INLINE_FUNCTION
	bool
	hasParent() const noexcept
	{
		return (_parentId != invalid<std::size_t>);
	}

	/*!
	 * @brief Get the index to the parent Zone
	 */
	KOKKOS_INLINE_FUNCTION
	std::size_t
	getParentIndex() const noexcept
	{
		return _parentId;
	}

private:
	//! Referenced Region
	RegionType _region;
	//! Subdivision level
	std::size_t _level{};
	//! Index to parent Zone
	std::size_t _parentId{invalid<std::size_t>};
	//! Interval of indices to subzones
	Interval<std::size_t> _subZoneIds;
	//! Index to owned Tile
	std::size_t _tileId{invalid<std::size_t>};
};
} // namespace plsm
