#pragma once

#include <plsm/Region.h>
#include <plsm/refine/Detector.h>

namespace plsm
{
namespace refine
{
template <typename TScalar, std::size_t Dim, typename TTag = void>
class RegionDetector : public Detector<RegionDetector<TScalar, Dim, TTag>, TTag>
{
public:
    using Superclass = Detector<RegionDetector<TScalar, Dim, TTag>, TTag>;
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
        bool ret = false;
        for (std::size_t i = 0; i < region.dimension(); ++i) {
            auto ival0 = region[i];
            auto ival1 = _region[i];
            if (!ival0.intersects(ival1)) {
                ret = false;
                break;
            }
            if (intersectEndPoints(ival0, ival1)) {
                ret = true;
            }
        }
        return ret;
    }

    using Superclass::overlap;

    KOKKOS_INLINE_FUNCTION
    bool
    overlap(const RegionType& region) const
    {
        return _region.intersects(region);
    }

private:
    using IntervalType = typename RegionType::IntervalType;

    KOKKOS_INLINE_FUNCTION
    bool
    intersectEndPoints(const IntervalType& ival0, const IntervalType& ival1)
        const
    {
        if (ival0.empty() || ival1.empty()) {
            return false;
        }
        auto last0 = ival0.end() - 1;
        auto last1 = ival1.end() - 1;
        return (ival0.begin() < ival1.begin() && last0 > ival1.begin()) ||
            (ival0.begin() < last1 && ival0.end() > last1);
    }

private:
    RegionType _region;
};
}
}
