#pragma once

#include <algorithm>
#include <cmath>

#include <plsm/Region.h>
#include <plsm/refine/Detector.h>

namespace plsm
{
namespace refine
{
/*!
 * BallDetector is a Detector implementing intersect() and overlap() with
 * respect to a hyperball
 *
 * @test unittest_Detectors
 * @test benchmark_Subpaving.cpp
 */
template <typename TScalar, std::size_t Dim, typename TTag = void>
class BallDetector : public Detector<BallDetector<TScalar, Dim, TTag>, TTag>
{
public:
    //! Alias for parent class type
    using Superclass = Detector<BallDetector<TScalar, Dim, TTag>, TTag>;
    //! Underlying lattice scalar type
    using ScalarType = TScalar;
    //! Type to use for scalar differences
    using ScalarDiff = DifferenceType<ScalarType>;
    //! Spatial point representation
    using PointType = SpaceVector<ScalarType, Dim>;
    //! Alias for Region
    using RegionType = Region<ScalarType, Dim>;

    using Superclass::Superclass;

    /*!
     * @brief Construct with ball center and radius
     * @param center Ball center point
     * @param radius Ball radius
     * @param refineDepth Level limit on refinement (defaults to
     * Detector::fullDepth)
     */
    BallDetector(const PointType& center, const ScalarType& radius,
            std::size_t refineDepth = Superclass::fullDepth)
        :
        Superclass(refineDepth),
        _center{center},
        _radius{radius},
        _radSq{radius*radius}
    {
    }

    using Superclass::intersect;

    /*!
     * @brief Test for intersection of given Region with hyperball boundary 
     */
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
                auto r = static_cast<ScalarType>(plsm::max(e_lo, plsm::abs(e_hi)));
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

    /*!
     * @brief Test for if the given Region either intersects or is contained by
     * the hyperball
     */
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

private:
    //! Ball center point
    PointType _center{};
    //! Ball radius
    ScalarType _radius{};
    //! Square of ball radius
    ScalarType _radSq{};
};
}
}
