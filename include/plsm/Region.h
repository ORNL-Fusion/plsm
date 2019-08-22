#pragma once

//std
#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <vector>
//Kokkos
#include <Kokkos_Array.hpp>
#include <Kokkos_Core.hpp>
//plsm
#include <plsm/CompactFlat.h>
#include <plsm/Interval.h>
#include <plsm/Segment.h>
#include <plsm/SpaceVector.h>

namespace plsm
{
template <typename TScalar, std::size_t Dim>
class Region : public Kokkos::Array<Interval<TScalar>, Dim>
{
public:
    using ScalarType = TScalar;
    using IntervalType = Interval<ScalarType>;
    using Superclass = Kokkos::Array<IntervalType, Dim>;
    using VectorType = SpaceVector<ScalarType, Dim>;
    using FlatType = CompactFlat<ScalarType, Dim>;

    using Superclass::Superclass;

    KOKKOS_INLINE_FUNCTION
    Region() noexcept
        = default;

    KOKKOS_INLINE_FUNCTION
    Region(std::initializer_list<IntervalType> ilist) noexcept
    {
        assert(ilist.size() == Dim);
        for (std::size_t i = 0; i < Dim; ++i) {
            (*this)[i] = ilist.begin()[i];
        }
    }

    static
    KOKKOS_INLINE_FUNCTION
    constexpr std::size_t
    dimension() noexcept
    {
        return Dim;
    }

    KOKKOS_INLINE_FUNCTION
    bool
    empty() const noexcept
    {
        bool ret = false;
        for (auto ival : *this) {
            if (ival.empty()) {
                ret = true;
                break;
            }
        }
        return ret;
    }

    KOKKOS_INLINE_FUNCTION
    bool
    contains(const VectorType& p) const;

    KOKKOS_INLINE_FUNCTION
    bool
    intersects(const FlatType& p) const;

    KOKKOS_INLINE_FUNCTION
    bool
    intersects(const FlatType& a, const FlatType& b) const;

    template <typename TPoint>
    KOKKOS_INLINE_FUNCTION
    bool
    intersects(Kokkos::View<TPoint*> points) const;

    bool
    intersects(const std::vector<VectorType>& points) const;

    KOKKOS_INLINE_FUNCTION
    bool
    intersects(const Region& other) const;

    template <RangeElem Elem>
    KOKKOS_INLINE_FUNCTION
    bool
    intersectsFace(std::size_t spaceCoord, const Segment<FlatType>& segment)
        const;
};
} /* namespace plsm */

#include <plsm/Region.inl>
