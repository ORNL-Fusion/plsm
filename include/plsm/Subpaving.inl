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
template <typename TScalar, std::size_t Dim, typename TEnum, typename TItemData>
Subpaving<TScalar, Dim, TEnum, TItemData>::Subpaving(const RegionType& region,
        const std::vector<SubdivisionRatioType>& subdivisionRatios)
    :
    _zones("zones", 1),
    _tiles("tiles", 1),
    _rootRegion(region)
{
    processSubdivisionRatios(subdivisionRatios);

    _zones.h_view(0) = ZoneType{_rootRegion, 0};
    _zones.modify_host();

    _tiles.h_view(0) = TileType{_rootRegion, 0};
    _tiles.modify_host();

    _refinementDepth = 1;

    _zones.sync_device();
    _tiles.sync_device();
}


template <typename TScalar, std::size_t Dim, typename TEnum, typename TItemData>
void
Subpaving<TScalar, Dim, TEnum, TItemData>::processSubdivisionRatios(
    const std::vector<SubdivisionRatioType>& subdivRatios)
{
    auto subdivisionRatios = subdivRatios;

    auto elementWiseProduct =
        [](const SubdivisionRatioType& a, const SubdivisionRatioType& b)
        {
            SubdivisionRatioType ret;
            for (auto i : makeIntervalRange(Dim)) {
                ret[i] = a[i] * b[i];
            }
            return ret;
        };
    auto ratioProduct =
        std::accumulate(next(begin(subdivisionRatios)), end(subdivisionRatios),
            subdivisionRatios.front(), elementWiseProduct);

    auto getIntervalLength =
        [](const IntervalType& ival) { return ival.length(); };
    std::array<typename IntervalType::SizeType, Dim> extents;
    std::transform(begin(_rootRegion), end(_rootRegion), begin(extents),
        getIntervalLength);

    //FIXME: infinite loop when ratios do not evenly divide extents
    for (;;) {
        bool needAnotherLevel = false;
        auto newRatio = SubdivisionRatioType::filled(1);
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

    _subdivisionInfos = Kokkos::DualView<detail::SubdivisionInfo<Dim>*>{
        "Subdivision Infos", subdivisionRatios.size()};
    std::copy(begin(subdivisionRatios), end(subdivisionRatios),
        _subdivisionInfos.h_view.data());
    _subdivisionInfos.modify_host();
    _subdivisionInfos.sync_device();
}


template <typename TScalar, std::size_t Dim, typename TEnum, typename TItemData>
template <typename TRefinementDetector>
void
Subpaving<TScalar, Dim, TEnum, TItemData>::refine(
    TRefinementDetector&& detector)
{
    detail::makeRefiner(*this, std::forward<TRefinementDetector>(detector))();
}


template <typename TScalar, std::size_t Dim, typename TEnum, typename TItemData>
template <typename TContext>
KOKKOS_INLINE_FUNCTION
std::size_t
Subpaving<TScalar, Dim, TEnum, TItemData>::findTileId(const PointType& point,
    TContext context) const
{
    auto zones = getZones(context);
    std::size_t zoneId = 0;
    auto zone = zones(zoneId);
    auto tileId = invalid<std::size_t>;
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
            if (zones(subZoneId).getRegion().contains(point)) {
                newZoneId = subZoneId;
                break;
            }
        }
        if (newZoneId == zoneId) {
            break;
        }
        zoneId = newZoneId;
        zone = zones(zoneId);
    }
    return tileId;
}


template <typename TScalar, std::size_t Dim, typename TEnum, typename TItemData>
void
Subpaving<TScalar, Dim, TEnum, TItemData>::plot()
{
    auto tiles = getTiles();
    std::ofstream ofs("gp.txt");
    for (auto i : makeIntervalRange(tiles.extent(0))) {
        const auto& region = tiles(i).getRegion();
        ofs << "\n";
        ofs << region[0].begin() << " " << region[1].begin() << "\n";
        ofs << region[0].end() << " " << region[1].begin() << "\n";
        ofs << region[0].end() << " " << region[1].end() << "\n";
        ofs << region[0].begin() << " " << region[1].end() << "\n";
        ofs << region[0].begin() << " " << region[1].begin() << "\n";
        ofs << "\n";
        double q01 = 0.25*region[0].begin() + 0.75*region[0].end();
        double q03 = 0.75*region[0].begin() + 0.25*region[0].end();
        double q11 = 0.25*region[1].begin() + 0.75*region[1].end();
        double q13 = 0.75*region[1].begin() + 0.25*region[1].end();
        ofs << q01 << " " << q11 << "\n";
        ofs << q03 << " " << q13 << "\n";
        ofs << "\n";
        ofs << q01 << " " << q13 << "\n";
        ofs << q03 << " " << q11 << "\n";
    }
}
}
