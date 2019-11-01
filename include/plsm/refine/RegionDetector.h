#pragma once

#include <plsm/Region.h>
#include <plsm/refine/Detector.h>

namespace plsm
{
namespace refine
{
template <typename TScalar, std::size_t Dim, typename... Tags>
class RegionDetector :
    public Detector<RegionDetector<TScalar, Dim, Tags...>, Tags...>
{
public:
    using Superclass = Detector<RegionDetector<TScalar, Dim, Tags...>, Tags...>;
    using ScalarType = TScalar;
    using RegionType = Region<ScalarType, Dim>;

    using Superclass::Superclass;

    RegionDetector(const RegionType& region,
            std::size_t refineDepth = Superclass::fullDepth)
        :
        Superclass(refineDepth),
        _region{region}
    {
    }

    KOKKOS_INLINE_FUNCTION
    bool
    intersect(const RegionType& region) const
    {
        return _region.intersects(region);
    }

    KOKKOS_INLINE_FUNCTION
    bool
    select(const RegionType& region) const
    {
        return _region.intersects(region);
    }

private:
    RegionType _region;
};
}
}
