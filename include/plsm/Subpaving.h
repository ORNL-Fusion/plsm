#pragma once

#include <type_traits>
#include <vector>

#include <Kokkos_Core.hpp>
#include <Kokkos_DualView.hpp>

#include <plsm/ContextUtility.h>
#include <plsm/EnumIndexed.h>
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
template <typename TScalar, std::size_t Dim, typename TEnumIndex = void,
	typename TItemData = std::size_t>
class Subpaving
{
	template <typename TSubpaving, typename TSelector>
	friend class detail::Refiner;

public:
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
	//! The type for the set of zones (host/device synchronized)
	using ZonesDualView = Kokkos::DualView<ZoneType*>;
	//! The type for the set of zones on the given memory space
	template <typename TContext>
	using ZonesView = detail::ContextualViewType<ZonesDualView, TContext>;

	//! The subpaving Tile
	using TileType = Tile<RegionType, ItemDataType>;
	//! The type for the set of tiles (host/device synchronized)
	using TilesDualView = Kokkos::DualView<TileType*>;
	//! The type for the set of tiles on the given memory space
	template <typename TContext>
	using TilesView = detail::ContextualViewType<TilesDualView, TContext>;

	Subpaving() = delete;

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
	constexpr std::size_t
	dimension() noexcept
	{
		return Dim;
	}

	/*!
	 * @brief Get the invalid index value
	 */
	static KOKKOS_INLINE_FUNCTION
	constexpr std::size_t
	invalidIndex() noexcept
	{
		return invalid<std::size_t>;
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
	 * @brief Get tiles DualView
	 * @todo Rename this
	 */
	TilesDualView
	getTilesView()
	{
		return _tiles;
	}

	/*!
	 * @brief Synchronize Tile data onto specified memory space
	 */
	template <typename TContext = OnHost>
	void
	syncTiles(TContext context = onHost)
	{
		detail::syncUpdate(_tiles, context);
	}

	/*!
	 * @brief Synchronize Zone data onto specified memory space
	 */
	template <typename TContext = OnHost>
	void
	syncZones(TContext context = onHost)
	{
		detail::syncUpdate(_zones, context);
	}

	/*!
	 * @brief Synchronize Zone and Tile data onto specified memory space
	 */
	template <typename TContext = OnHost>
	void
	syncAll(TContext context = onHost)
	{
		syncTiles(context);
		syncZones(context);
	}

	/*!
	 * @brief Get current number of tiles held in the specified memory space
	 * @note This does not synchronize
	 */
	template <typename TContext = OnHost>
	std::size_t
	getNumberOfTiles(TContext context = onHost)
	{
		return getTiles(context).extent(0);
	}

	/*!
	 * @brief Get the set of tiles in the specified memory space
	 * @note This does not synchronize
	 */
	template <typename TContext = OnHost>
	KOKKOS_INLINE_FUNCTION
	const TilesView<TContext>&
	getTiles(TContext context = onHost) const
	{
		return detail::getContextualView(_tiles, context);
	}

	/*!
	 * @brief Get the set of zones in the specified memory space
	 * @note This does not synchronize
	 */
	template <typename TContext = OnHost>
	KOKKOS_INLINE_FUNCTION
	const ZonesView<TContext>&
	getZones(TContext context = onHost) const
	{
		return detail::getContextualView(_zones, context);
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
	template <typename TContext = OnHost>
	KOKKOS_INLINE_FUNCTION
	std::size_t
	findTileId(const PointType& point, TContext context = onHost) const;

	/*! @cond */
	void
	plot();
	/*! @endcond */

private:
	/*!
	 * @brief Check subdivision ratios for domain divisibility and copy final
	 * form (one per level) into device view
	 */
	void
	processSubdivisionRatios(const std::vector<SubdivisionRatio<Dim>>&);

private:
	//! Zones represent the entire subdivision tree for the root region
	ZonesDualView _zones;
	//! Tiles represent the (selected) leaf nodes of the tree
	TilesDualView _tiles;
	//! Region which fully encloses the domain of interest
	RegionType _rootRegion;
	//! Collection of SubdivisionInfo, one per expected refinement level
	Kokkos::DualView<detail::SubdivisionInfo<Dim>*> _subdivisionInfos;
	//! Level limit
	std::size_t _refinementDepth{};
};
} // namespace plsm

#include <plsm/Subpaving.inl>
