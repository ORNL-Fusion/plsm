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
/*!
 * @brief Region represents an axis-aligned bounding box in terms of a set of
 * Dim Interval(s)
 *
 * Region inherits Kokkos::Array<Interval<TScalar>, Dim> as its representation,
 * and is therefore indexable through the Kokkos::Array interface.
 *
 * @tparam TScalar Underlying scalar representation
 * @tparam Dim Dimension of lattice; size of array representation
 *
 * @test test_Region.cpp
 */
template <typename TScalar, std::size_t Dim>
class Region : public Kokkos::Array<Interval<TScalar>, Dim>
{
public:
    //! Underlying scalar representation
    using ScalarType = TScalar;
    //! Alias for Interval
    using IntervalType = Interval<ScalarType>;
    //! Alias for parent class type
    using Superclass = Kokkos::Array<IntervalType, Dim>;
    //! Alias for SpaceVector
    using VectorType = SpaceVector<ScalarType, Dim>;
    //! Alias for CompactFlat
    using FlatType = CompactFlat<ScalarType, Dim>;

    using Superclass::Superclass;

    /*!
     * @brief Default construct empty Region
     */
    KOKKOS_INLINE_FUNCTION
    Region() noexcept
        = default;

    /*!
     * @brief Construct from initializer_list of Interval
     */
    KOKKOS_INLINE_FUNCTION
    Region(std::initializer_list<IntervalType> ilist) noexcept
    {
        assert(ilist.size() == Dim);
        for (std::size_t i = 0; i < Dim; ++i) {
            (*this)[i] = ilist.begin()[i];
        }
    }

    /*!
     * @brief Dimension of lattice
     */
    static
    KOKKOS_INLINE_FUNCTION
    constexpr std::size_t
    dimension() noexcept
    {
        return Dim;
    }

    /*!
     * @brief Check if Region is empty (at least one Interval is empty)
     */
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

    /*!
     * @brief Check if Region is of length exactly one in each dimension, that
     * is, it comprises just a single point
     */
    KOKKOS_INLINE_FUNCTION
    bool
    isSimplex() const noexcept
    {
        bool ret = true;
        for (auto ival : *this) {
            if (ival.length() != 1) {
                ret = false;
                break;
            }
        }
        return ret;
    }

    /*!
     * @brief Check if the given point is inside the Region
     */
    KOKKOS_INLINE_FUNCTION
    bool
    contains(const VectorType& p) const;

    /*!
     * @brief Check if the Region overlaps the given flat
     * @todo Change name to overlaps
     */
    KOKKOS_INLINE_FUNCTION
    bool
    intersects(const FlatType& p) const;

    /*!
     * @brief Check if the Region overlaps the given "flat line" from a to b.
     *
     * This can mean different things depending on the dimension of the flats.
     *
     * @todo This needs to be explained
     *
     * @note Parameters a and b are assumed to correspond with respect to the
     * coordinates that are specified.
     */
    KOKKOS_INLINE_FUNCTION
    bool
    intersects(const FlatType& a, const FlatType& b) const;

    //@{
    /*!
     * @brief Check if the Region overlaps the given "flat polyline"
     */
    template <typename TPoint>
    KOKKOS_INLINE_FUNCTION
    bool
    intersects(Kokkos::View<TPoint*> points) const;

    bool
    intersects(const std::vector<VectorType>& points) const;
    //@}

    /*!
     * @brief Check if the Region overlaps with another Region
     */
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
