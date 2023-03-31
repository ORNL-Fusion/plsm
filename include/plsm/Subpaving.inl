#pragma once

#include <algorithm>
#include <array>
#include <fstream>
#include <numeric>
#include <stdexcept>
#include <utility>

#include <plsm/IntervalRange.h>
#include <plsm/detail/Refiner.h>

namespace plsm
{
template <typename TScalar, DimType Dim, typename TEnum, typename TItemData,
	typename TMemSpace>
Subpaving<TScalar, Dim, TEnum, TItemData, TMemSpace>::Subpaving(
	const RegionType& region,
	const std::vector<SubdivisionRatio<Dim>>& subdivisionRatios) :
	_zones("zones", 1),
	_zonesRA(_zones),
	_tiles("tiles", 1),
	_tilesRA(_tiles),
	_rootRegion(region)
{
	processSubdivisionRatios(subdivisionRatios);

	auto zonesMirror = create_mirror_view(_zones);
	zonesMirror[0] = ZoneType{_rootRegion, 0};
	deep_copy(_zones, zonesMirror);

	auto tilesMirror = create_mirror_view(_tiles);
	tilesMirror[0] = TileType{_rootRegion, 0};
	deep_copy(_tiles, tilesMirror);
}

template <typename TScalar, DimType Dim, typename TEnum, typename TItemData,
	typename TMemSpace>
void
Subpaving<TScalar, Dim, TEnum, TItemData, TMemSpace>::processSubdivisionRatios(
	const std::vector<SubdivisionRatio<Dim>>& subdivRatios)
{
	auto subdivisionRatios = subdivRatios;

	auto elementWiseProduct = [](const SubdivisionRatio<Dim>& a,
								  const SubdivisionRatio<Dim>& b) {
		SubdivisionRatio<Dim> ret;
		for (auto i : makeIntervalRange(Dim)) {
			ret[i] = a[i] * b[i];
		}
		return ret;
	};

	// Compute per-dimension ratio product for provided subdivision ratios
	auto ratioProduct =
		std::accumulate(begin(subdivisionRatios), end(subdivisionRatios),
			SubdivisionRatio<Dim>::filled(1), elementWiseProduct);

	// Get root region extents (what is being subdivided)
	std::array<typename IntervalType::SizeType, Dim> extents;
	std::transform(begin(_rootRegion), end(_rootRegion), begin(extents),
		[](auto&& ival) { return ival.length(); });

	auto nonSelfFactors = [](auto x) {
		using T = std::remove_reference_t<decltype(x)>;
		std::vector<T> result;

		// This will loop from 2 to sqrt(x)
		for (T i = 2; i * i <= x; ++i) {
			// Check if i divides x without leaving a remainder
			if (x % i == 0) {
				result.push_back(i);
				// Include other factor if not root
				if (x / i != i) {
					result.push_back(x / i);
				}
			}
		}

		std::sort(begin(result), end(result));
		return result;
	};

	auto getNextFactor = [nonSelfFactors](auto toSub, auto refFactor) {
		using T = std::remove_reference_t<decltype(refFactor)>;
		if (toSub % refFactor == 0) {
			return refFactor;
		}
		auto opt = nonSelfFactors(static_cast<T>(toSub));
		if (opt.empty()) {
			return static_cast<T>(toSub);
		}
		T ret = 0;
		for (auto i : opt) {
			if (i > refFactor) {
				continue;
			}
			ret = i;
		}
		if (ret == 0) {
			ret = opt.front();
		}
		return ret;
	};

	// Create additional ratio(s) until space is fully subdivided
	for (;;) {
		bool needAnotherLevel = false;
		auto nextRatio = SubdivisionRatio<Dim>::filled(1);
		for (auto i : makeIntervalRange(Dim)) {
			if (ratioProduct[i] < extents[i]) {
				// Determine next ratio to use to subdivide leftovers. Use last
				// ratio if possible
				nextRatio[i] = getNextFactor(
					extents[i] / ratioProduct[i], subdivisionRatios.back()[i]);
				needAnotherLevel = true;
			}
		}

		if (!needAnotherLevel) {
			break;
		}

		subdivisionRatios.push_back(nextRatio);
		ratioProduct = elementWiseProduct(ratioProduct, nextRatio);
	}

	for (auto i : makeIntervalRange(Dim)) {
		if (ratioProduct[i] != extents[i]) {
			throw std::invalid_argument(
				"Subpaving: subdivision ratios given for dimension " +
				std::to_string(i) + " (" + std::to_string(ratioProduct[i]) +
				") do not match extent for that dimension (" +
				std::to_string(extents[i]) + ")");
		}
	}

	_subdivisionInfos =
		Kokkos::View<detail::SubdivisionInfo<Dim>*, MemorySpace>{
			"Subdivision Infos", subdivisionRatios.size()};
	auto subdivInfoMirror = create_mirror_view(_subdivisionInfos);
	std::copy(begin(subdivisionRatios), end(subdivisionRatios),
		subdivInfoMirror.data());
	deep_copy(_subdivisionInfos, subdivInfoMirror);
}

template <typename TScalar, DimType Dim, typename TEnum, typename TItemData,
	typename TMemSpace>
typename Subpaving<TScalar, Dim, TEnum, TItemData, TMemSpace>::HostMirror
Subpaving<TScalar, Dim, TEnum, TItemData, TMemSpace>::makeMirrorCopy() const
{
	HostMirror ret{};
	auto zones = create_mirror_view(_zones);
	deep_copy(zones, _zones);
	ret.setZones(zones);

	auto tiles = create_mirror_view(_tiles);
	deep_copy(tiles, _tiles);
	ret.setTiles(tiles);

	ret._rootRegion = _rootRegion;

	resize(ret._subdivisionInfos, _subdivisionInfos.size());
	deep_copy(ret._subdivisionInfos, _subdivisionInfos);

	ret._refinementDepth = _refinementDepth;

	return ret;
}

template <typename TScalar, DimType Dim, typename TEnum, typename TItemData,
	typename TMemSpace>
std::uint64_t
Subpaving<TScalar, Dim, TEnum, TItemData, TMemSpace>::getDeviceMemorySize()
	const noexcept
{
	std::uint64_t ret{};

	ret += _tiles.required_allocation_size(_tiles.size());
	ret += _zones.required_allocation_size(_zones.size());
	ret += sizeof(_rootRegion);
	ret += _subdivisionInfos.required_allocation_size(_subdivisionInfos.size());
	ret += sizeof(_refinementDepth);

	return ret;
}

template <typename TScalar, DimType Dim, typename TEnum, typename TItemData,
	typename TMemSpace>
template <typename TRefinementDetector>
void
Subpaving<TScalar, Dim, TEnum, TItemData, TMemSpace>::refine(
	TRefinementDetector&& detector)
{
	using Refiner = detail::Refiner<Subpaving, TRefinementDetector>;
	auto refiner = Refiner{*this, std::forward<TRefinementDetector>(detector)};
	refiner();
}

template <typename TScalar, DimType Dim, typename TEnum, typename TItemData,
	typename TMemSpace>
KOKKOS_INLINE_FUNCTION
IdType
Subpaving<TScalar, Dim, TEnum, TItemData, TMemSpace>::findTileId(
	const PointType& point) const
{
	IdType zoneId = 0;
	auto zone = _zonesRA(zoneId);
	auto tileId = invalid<IdType>;
	if (!zone.getRegion().contains(point)) {
		return tileId;
	}
	for (;;) {
		if (zone.hasTile()) {
			tileId = zone.getTileIndex();
			break;
		}
		auto newZoneId = zoneId;
		for (auto subZoneId : zone.getSubZoneRange()) {
			if (_zonesRA(subZoneId).getRegion().contains(point)) {
				newZoneId = subZoneId;
				break;
			}
		}
		if (newZoneId == zoneId) {
			break;
		}
		zoneId = newZoneId;
		zone = _zonesRA(zoneId);
	}
	return tileId;
}
} // namespace plsm
