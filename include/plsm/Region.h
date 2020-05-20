#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <vector>

#include <Kokkos_Array.hpp>
#include <Kokkos_Core.hpp>

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
    using ArrayType = Kokkos::Array<IntervalType, Dim>;
    //! Alias for SpaceVector
    using VectorType = SpaceVector<ScalarType, Dim>;
    //! Alias for CompactFlat
    using FlatType = CompactFlat<ScalarType, Dim>;

    using ArrayType::ArrayType;

    /*!
     * @brief Default construct empty Region
     */
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
     * @brief Construct from parent array type
     */
    KOKKOS_INLINE_FUNCTION
    Region(const ArrayType& arr) noexcept
        :
        ArrayType(arr)
    {
    }

    /*!
     * @brief Construct single point region at the given point
     */
    explicit
    KOKKOS_INLINE_FUNCTION
    Region(const VectorType& point) noexcept
    {
        for (std::size_t i = 0; i < Dim; ++i) {
            (*this)[i] = IntervalType{point[i], point[i]+1};
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
     * @brief Compute volume, that is, the number of lattice points contained
     */
    KOKKOS_INLINE_FUNCTION
    typename IntervalType::SizeType
    volume() const noexcept
    {
        typename IntervalType::SizeType ret{1};
        for (auto ival : *this) {
            ret *= ival.length();
        }
        return ret;
    }

    /*!
     * @brief Compute dispersion (sigma^2)
     * @todo Figure out what this means :)
     */
    KOKKOS_INLINE_FUNCTION
    SpaceVector<double, Dim>
    dispersion() const noexcept;

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
     * @brief Construct the point at the lower limit
     */
    KOKKOS_INLINE_FUNCTION
    VectorType
    getOrigin() const noexcept
    {
        VectorType ret;
        for (std::size_t i = 0; i < Dim; ++i) {
            ret[i] = (*this)[i].begin();
        }
        return ret;
    }

    /*!
     * @brief Construct the point at the upper limit
     */
    KOKKOS_INLINE_FUNCTION
    VectorType
    getUpperLimitPoint() const noexcept
    {
        VectorType ret;
        for (std::size_t i = 0; i < Dim; ++i) {
            ret[i] = (*this)[i].end();
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

    /*!
     * @brief Check if the given flat segment crosses face in the given
     * dimension
     */
    template <RangeElem Elem>
    KOKKOS_INLINE_FUNCTION
    bool
    intersectsFace(std::size_t flatCoord, const Segment<FlatType>& segment)
        const;
};

/*!
 * @relates Region
 * @brief Insert a Region to an output stream
 */
template <typename T, std::size_t N>
inline
std::ostream&
operator<<(std::ostream& os, const Region<T, N>& r)
{
    os << "{";
    for (std::size_t i = 0; i < N; ++i) {
        os << " " << r[i];
    }
    os << " }";
    return os;
}

/*!
 * @relates Region
 * @brief Check equality of two Regions, potentially using different limit types
 */
template <typename T, typename U, std::size_t N>
KOKKOS_INLINE_FUNCTION
bool
operator==(const Region<T, N>& a, const Region<U, N>& b) noexcept
{
    bool ret = true;
    for (std::size_t i = 0; i < N; ++i) {
        if (a[i] != b[i]) {
            ret = false;
            break;
        }
    }
    return ret;
}

/*!
 * @relates Region
 * @brief Two Regions are not equal
 */
template <typename T, typename U, std::size_t N>
KOKKOS_INLINE_FUNCTION
bool
operator!=(const Region<T, N>& a, const Region<U, N>& b) noexcept
{
    return !(a == b);
}
} /* namespace plsm */

#include <plsm/Region.inl>
