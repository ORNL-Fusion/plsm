#pragma once

//std
#include <algorithm>
#include <array>
#include <fstream>
#include <numeric>
#include <stdexcept>
#include <utility>
//plsm
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
    // ,
    //FIXME:
    //      Cuda runtime error if left "empty".
    //      KOKKOS_INVALID_INDEX is used by default which is a huge number.
    //      This should be reported once we get back to kokkos master branch.
    // _subdivisionInfos("Subdivision Infos", 1)
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

    _subdivisionInfos = Kokkos::DualView<SubdivisionInfoType*>{
        "Subdivision Infos", subdivisionRatios.size()};
    // Kokkos::resize(_subdivisionInfos.h_view, subdivisionRatios.size());
    std::copy(begin(subdivisionRatios), end(subdivisionRatios),
        _subdivisionInfos.h_view.data());
    _subdivisionInfos.modify_host();
    Kokkos::resize(_subdivisionInfos.d_view, subdivisionRatios.size());
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
typename Subpaving<TScalar, Dim, TEnum, TItemData>::TilesHostView
Subpaving<TScalar, Dim, TEnum, TItemData>::getTilesOnHost()
{
    _tiles.modify_device();
    Kokkos::resize(_tiles.h_view, _tiles.d_view.extent(0));
    _tiles.sync_host();
    return _tiles.h_view;
}


template <typename TScalar, std::size_t Dim, typename TEnum, typename TItemData>
typename Subpaving<TScalar, Dim, TEnum, TItemData>::ZonesHostView
Subpaving<TScalar, Dim, TEnum, TItemData>::getZonesOnHost()
{
    _zones.modify_device();
    Kokkos::resize(_zones.h_view, _zones.d_view.extent(0));
    _zones.sync_host();
    return _zones.h_view;
}


template <typename TScalar, std::size_t Dim, typename TEnum, typename TItemData>
std::size_t
Subpaving<TScalar, Dim, TEnum, TItemData>::getTileId(const PointType& point)
{
    auto zones = getZonesOnHost();
    return getTileId(point, zones(0));
}


template <typename TScalar, std::size_t Dim, typename TEnum, typename TItemData>
std::size_t
Subpaving<TScalar, Dim, TEnum, TItemData>::getTileId(const PointType& point,
    const ZoneType& zone) const
{
    //
    //TODO: Abstract this process so it can be performed on device or host
    //
    //
    //  getZones<Device>() -> _zones.d_view
    //  getZones<Host>() -> _zones.h_view
    //
    //  template <Context>
    //  getTileId()
    //  {
    //      auto zones = getZones<Context>();
    //      ...
    //  }
    //

    if (zone.getRegion().contains(point)) {
        if (zone.hasTile()) {
            return zone.getTileIndex();
        }
        for (auto subZoneId : zone.getSubZoneRange()) {
            auto tileId = getTileId(point, _zones.h_view(subZoneId));
            if (tileId != invalid<std::size_t>) {
                return tileId;
            }
        }
    }
    return invalid<std::size_t>;
}


template <typename TScalar, std::size_t Dim, typename TEnum, typename TItemData>
void
Subpaving<TScalar, Dim, TEnum, TItemData>::plot()
{
    auto tiles = getTilesOnHost();
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
