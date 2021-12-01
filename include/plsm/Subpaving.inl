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
	auto ratioProduct = std::accumulate(next(begin(subdivisionRatios)),
		end(subdivisionRatios), subdivisionRatios.front(), elementWiseProduct);

	auto getIntervalLength = [](const IntervalType& ival) {
		return ival.length();
	};
	std::array<typename IntervalType::SizeType, Dim> extents;
	std::transform(begin(_rootRegion), end(_rootRegion), begin(extents),
		getIntervalLength);

	// FIXME: infinite loop when ratios do not evenly divide extents
	for (;;) {
		bool needAnotherLevel = false;
		auto newRatio = SubdivisionRatio<Dim>::filled(1);
		for (auto i : makeIntervalRange(Dim)) {
			if (ratioProduct[i] < extents[i]) {
				newRatio[i] = subdivisionRatios.back()[i];
				needAnotherLevel = true;
			}
		}

		if (!needAnotherLevel) {
			break;
		}

		subdivisionRatios.push_back(newRatio);
		ratioProduct = elementWiseProduct(ratioProduct, newRatio);
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
