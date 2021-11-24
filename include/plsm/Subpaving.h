#pragma once

#include <type_traits>
#include <vector>

#include <Kokkos_Core.hpp>

#include <plsm/EnumIndexed.h>
#include <plsm/Utility.h>
#include <plsm/Zone.h>
#include <plsm/detail/Refiner.h>
#include <plsm/detail/SubdivisionInfo.h>

namespace plsm
{
/*!
 * @brief A set of non-overlapping "tiles" which cover the space within a given
 * Region of an N-dimensional lattice
 *
 * This data structure can refine according to a highly customizable
 * specification, including the ability to discard tiles that are, for example,
 * outside an area of interest. Customization is available through the
 * SubdivisionRatio instances provided to the constructor and through the
 * specific refine::Detector passed to refine().
 *
 * @tparam TScalar The underlying type for scalar representation for the lattice
 * @tparam Dim The dimension of the lattice
 * @tparam TEnumIndex An optional enum type to be used to index the space
 * @tparam TItemData An optional data type to associate with each Tile
 *
 * @test unittest_Subpaving.cpp
 * @test benchmark_Subpaving.cpp
 */
template <typename TScalar, DimType Dim, typename TEnumIndex = void,
	typename TItemData = IdType, typename TMemSpace = DefaultMemSpace>
class Subpaving
{
	template <typename TSubpaving, typename TSelector>
	friend class detail::Refiner;

	static_assert(Kokkos::is_memory_space<TMemSpace>{});

public:
	//! The memory space for the subpaving data
	using MemorySpace = TMemSpace;
	//! Underlying type for scalar representation for the lattice
	using ScalarType = TScalar;
	//! The type to represent lattice regions
	using RegionType = EnumIndexed<Region<ScalarType, Dim>, TEnumIndex>;
	//! The type to represent points in the lattice
	using PointType = EnumIndexed<SpaceVector<ScalarType, Dim>, TEnumIndex>;
	//! The type to represent half-open intervals in a single dimension
	using IntervalType = typename RegionType::IntervalType;
	//! The user data type to map from tiles
	using ItemDataType = TItemData;

	//! The subdivision Zone
	using ZoneType = Zone<RegionType>;
	//! The type for the set of zones on the given memory space
	using ZonesView = Kokkos::View<ZoneType*, MemorySpace>;

	//! The subpaving Tile
	using TileType = Tile<RegionType, ItemDataType>;
	//! The type for the set of tiles on the given memory space
	using TilesView = Kokkos::View<TileType*, MemorySpace>;

	using HostMirrorSpace = typename TilesView::traits::host_mirror_space;
	using HostMirror =
		Subpaving<TScalar, Dim, TEnumIndex, TItemData, HostMirrorSpace>;

private:
	template <typename, DimType, typename, typename, typename>
	friend class ::plsm::Subpaving;

public:
	Subpaving() = default;

	/*!
	 * @brief Construct from root Region and set of subdivision ratios.
	 *
	 * The subdivision ratios will be processed in order to fully refine the
	 * space. That is, for each dimension of the lattice, if the set of ratios
	 * given would not fully refine the space, the last ratio given will be
	 * repeated until the space would be fully refined. If this ratio cannot
	 * evenly subdivide the space until it is fully refined, an exception is
	 * thrown and Subpaving construction is aborted.
	 */
	Subpaving(const RegionType& region,
		const std::vector<SubdivisionRatio<Dim>>& subdivisionRatios);

	//!@{
	/*!
	 * Default copy operations
	 */
	Subpaving(const Subpaving&) = default;
	Subpaving&
	operator=(const Subpaving&) = default;
	//!@}

	/*!
	 * @brief Get the dimension of the lattice
	 */
	static KOKKOS_INLINE_FUNCTION
	constexpr DimType
	dimension() noexcept
	{
		return Dim;
	}

	/*!
	 * @brief Get the invalid index value
	 */
	static KOKKOS_INLINE_FUNCTION
	constexpr IdType
	invalidIndex() noexcept
	{
		return invalid<IdType>;
	}

	HostMirror
	makeMirrorCopy() const
	{
		HostMirror ret{};
		resize(ret._zones, _zones.size());
		deep_copy(ret._zones, _zones);
		resize(ret._tiles, _tiles.size());
		deep_copy(ret._tiles, _tiles);
		ret._rootRegion = _rootRegion;
		resize(ret._subdivisionInfos, _subdivisionInfos.size());
		ret._refinementDepth = _refinementDepth;
		return ret;
	}

	/*!
	 * @brief Get root region
	 */
	const RegionType&
	getLatticeRegion() const noexcept
	{
		return _rootRegion;
	}

	/*!
	 * @brief Get size (in bytes) of memory used on the device
	 */
	std::uint64_t
	getDeviceMemorySize() const noexcept;

	/*!
	 * @brief Get tiles View
	 * @todo Rename this
	 */
	const TilesView&
	getTiles() const
	{
		return _tiles;
	}

	/*!
	 * @brief Get current number of tiles
	 */
	IdType
	getNumberOfTiles() const
	{
		return static_cast<IdType>(_tiles.size());
	}

	/*!
	 * @brief Get the set of zones
	 */
	KOKKOS_INLINE_FUNCTION
	const ZonesView&
	getZones() const
	{
		return _zones;
	}

	/*!
	 * @brief Get the current refinement depth
	 */
	std::size_t
	getRefinementDepth() const noexcept
	{
		return _refinementDepth;
	}

	/*!
	 * @brief Refine the Subpaving according to the given refine::Detector
	 */
	template <typename TRefinementDetector>
	void
	refine(TRefinementDetector&& detector);

	/*!
	 * @brief Perform a tree search (using the zones) for the given point, and
	 * return the id of the containing tile (or invalid if not found)
	 */
	KOKKOS_INLINE_FUNCTION
	IdType
	findTileId(const PointType& point) const;

private:
	/*!
	 * @brief Check subdivision ratios for domain divisibility and copy final
	 * form (one per level) into device view
	 */
	void
	processSubdivisionRatios(const std::vector<SubdivisionRatio<Dim>>&);

private:
	//! Zones represent the entire subdivision tree for the root region
	ZonesView _zones;
	//! Tiles represent the (selected) leaf nodes of the tree
	TilesView _tiles;
	//! Region which fully encloses the domain of interest
	RegionType _rootRegion;
	//! Collection of SubdivisionInfo, one per expected refinement level
	Kokkos::View<detail::SubdivisionInfo<Dim>*, MemorySpace> _subdivisionInfos;
	//! Level limit
	std::size_t _refinementDepth{};
};

namespace detail
{
template <typename TMemSpace, typename TSubpaving>
struct MemSpaceSubpavingHelper;

template <typename TMemSpace, typename TS, DimType Dim, typename TE,
	typename TD, typename TM>
struct MemSpaceSubpavingHelper<TMemSpace, Subpaving<TS, Dim, TE, TD, TM>>
{
	using Type = Subpaving<TS, Dim, TE, TD, TMemSpace>;
};
} // namespace detail

template <typename TMemSpace, typename TSubpaving>
using MemSpaceSubpaving =
	typename detail::MemSpaceSubpavingHelper<TMemSpace, TSubpaving>::Type;
} // namespace plsm

#include <plsm/Subpaving.inl>
