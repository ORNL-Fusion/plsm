#pragma once

//std
#include <vector>
//Kokkos
#include <Kokkos_Core.hpp>
//plsm
#include <plsm/Region.h>
#include <plsm/refine/Detector.h>

namespace plsm
{
namespace refine
{
template <typename TScalar, std::size_t Dim, typename... Tags>
class PolylineDetector :
    public Detector<PolylineDetector<TScalar, Dim, Tags...>, Tags...>
{
public:
    using Superclass =
        Detector<PolylineDetector<TScalar, Dim, Tags...>, Tags...>;
    using ScalarType = TScalar;
    using PointType = SpaceVector<ScalarType, Dim>;
    using FlatType = CompactFlat<ScalarType, Dim>;
    using RegionType = Region<ScalarType, Dim>;

    using Superclass::Superclass;

    PolylineDetector(const std::vector<PointType>& polyline,
            std::size_t refineDepth = Superclass::fullDepth)
        :
        Superclass(refineDepth),
        _flats("Refinement Polyline Flats", polyline.size())
    {
        auto fMirror = Kokkos::create_mirror_view(_flats);
        std::copy(begin(polyline), end(polyline), fMirror.data());
        Kokkos::deep_copy(_flats, fMirror);
    }

    KOKKOS_INLINE_FUNCTION
    bool
    intersect(const RegionType& region) const
    {
        return region.intersects(_flats);
    }

    KOKKOS_INLINE_FUNCTION
    bool
    select(const RegionType& region) const
    {
        return (*this)(SelectAll{}, region);
    }

private:
    Kokkos::View<FlatType*> _flats;
};
}
}
