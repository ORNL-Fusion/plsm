#pragma once

//std
#include <algorithm>
#include <cmath>
//plsm
#include <plsm/Region.h>
#include <plsm/refine/Detector.h>

namespace plsm
{
namespace refine
{
template <typename TScalar, std::size_t Dim, typename... Tags>
class BallDetector :
    public Detector<BallDetector<TScalar, Dim, Tags...>, Tags...>
{
public:
    using Superclass = Detector<BallDetector<TScalar, Dim, Tags...>, Tags...>;
    using ScalarType = TScalar;
    using ScalarDiff = DifferenceType<ScalarType>;
    using PointType = SpaceVector<ScalarType, Dim>;
    using RegionType = Region<ScalarType, Dim>;

    using Superclass::Superclass;

    BallDetector(const PointType& center, const ScalarType& radius,
            std::size_t refineDepth = Superclass::fullDepth)
        :
        Superclass(refineDepth),
        _center{center},
        _radius{radius},
        _radSq{radius*radius}
    {
    }

    KOKKOS_INLINE_FUNCTION
    bool
    overlap(const RegionType& region) const
    {
        constexpr ScalarDiff zero = 0;
        auto rad = static_cast<ScalarDiff>(_radius);
        auto negRad = -rad;
        ScalarType d = 0;
        for (std::size_t i = 0; i < Dim; ++i) {
            auto c_i = static_cast<ScalarDiff>(_center[i]);
            auto r_lo_i = static_cast<ScalarDiff>(region[i].begin());
            auto r_hi_i = static_cast<ScalarDiff>(region[i].end());
            auto e = c_i - r_lo_i;
            if (e < zero) {
                if (e < negRad) {
                    return false;
                }
                d += static_cast<ScalarType>(e*e);
                continue;
            }
            e = c_i - r_hi_i;
            if (e > zero) {
                if (e > rad) {
                    return false;
                }
                d += static_cast<ScalarType>(e*e);
            }
        }
        return (d <= _radSq);
    }

    KOKKOS_INLINE_FUNCTION
    bool
    intersect(const RegionType& region) const
    {
        constexpr ScalarDiff zero = 0;
        auto rad = static_cast<ScalarDiff>(_radius);
        auto negRad = -rad;
        ScalarType d_min = 0;
        ScalarType d_max = 0;
        for (std::size_t i = 0; i < Dim; ++i) {
            auto c_i = static_cast<ScalarDiff>(_center[i]);
            auto r_lo_i = static_cast<ScalarDiff>(region[i].begin());
            auto r_hi_i = static_cast<ScalarDiff>(region[i].end());
            auto e_lo = c_i - r_lo_i;
            auto e_hi = c_i - r_hi_i;
            if (e_lo < zero) {
                if (e_lo < negRad) {
                    return false;
                }
                d_min += static_cast<ScalarType>(e_lo*e_lo);
                d_max += static_cast<ScalarType>(e_hi*e_hi);
            }
            else if (e_hi > zero) {
                if (e_hi > rad) {
                    return false;
                }
                d_min += static_cast<ScalarType>(e_hi*e_hi);
                d_max += static_cast<ScalarType>(e_lo*e_lo);
            }
            else {
                auto r = static_cast<ScalarType>(std::max(e_lo, std::abs(e_hi)));
                d_max += r*r;
            }
        }
        if (d_min == 0 && d_max > 4*_radSq) {
            return true;
        }
        if (d_min <= _radSq && d_max >= _radSq) {
            return true;
        }
        return false;
    }

    KOKKOS_INLINE_FUNCTION
    bool
    select(const RegionType& region) const
    {
        return overlap(region);
    }

private:
    PointType _center{};
    ScalarType _radius{};
    ScalarType _radSq{};
};
}
}
