#pragma once

#include <algorithm>
#include <cassert>
#include <limits>

#include <plsm/SpaceVector.h>
#include <plsm/Utility.h>

namespace plsm
{
/*!
 * @brief CompactFlat represents a "flat" subspace of a lattice
 *
 * A flat with respect to a vector space is a subspace of dimension less than
 * that of the parent space. CompactFlat represents this by specifying a vector
 * of the parent space with values at coordinates which are specified constant,
 * and a wildcard value elsewhere.
 *
 * For example, for a 3D space, a flat is a plane, line, or point. The xy-plane
 * can be specified as {wildcard, wildcard, 0}, that is, z held constant at 0.
 * The z axis can be specified as {0, 0, wildcard}. A z-oriented line passing
 * through (x=5, y=3) would be {5, 3, wildcard}.
 *
 * The representation is compact in that the specified coordinate values are
 * held in the first entries of the member array. So a CompactFlat constructed
 * from the vector {wildcard, 1, 2} would be represented as {1, 2, 'garbage'},
 * and the indexing operator returns values according to this ordering.
 * Therefore CompactFlat indices do not relate directly to the indices for the
 * parent space. But an index may be mapped to the parent space using
 * expandCoordinate(). Similarly, a parent space vector may be obtained (with
 * wildcard coordinate values as appropriate) using expand().
 *
 * @note The full parent space can be represented if all coordinate values are
 * wildcard. This is the state of a default constructed CompactFlat.
 *
 * @tparam TScalar The underlying scalar type
 * @tparam Dim The dimension of the parent space
 *
 * @test test_CompactFlat.cpp
 */
template <typename TScalar, std::size_t Dim>
class CompactFlat
{
    template <typename TScalar2, std::size_t Dim2>
    friend class CompactFlat;

public:
    //! Underlying scalar representation
    using ScalarType = TScalar;
    //! Type for representing a vector in the parent space
    using VectorType = SpaceVector<ScalarType, Dim>;

    /*!
     * @brief Default construction represents full space (size() == 0,
     * dimension() == Dim)
     */
    KOKKOS_INLINE_FUNCTION
    CompactFlat() noexcept
        :
        _size{0}
    {
    }

    /*!
     * @brief Construct from parent space vector
     */
    KOKKOS_INLINE_FUNCTION
    CompactFlat(const VectorType& vector) noexcept
        :
        _size{0}
    {
        for (std::size_t i = 0; i < Dim; ++i) {
            if (vector[i] == wildcard<TScalar>) {
                continue;
            }
            _rep[_size] = vector[i];
            _idMap[_size] = i;
            ++_size;
        }
    }

    /*!
     * @brief Copy from another CompactFlat, with potentially a different scalar
     * type
     */
    template <typename TScalar2>
    KOKKOS_INLINE_FUNCTION
    CompactFlat(const CompactFlat<TScalar2, Dim>& other)
        :
        _idMap{other._idMap},
        _size{other._size}
    {
        for (std::size_t i = 0; i < _size; ++i) {
            _rep[i] = static_cast<ScalarType>(other._rep[i]);
        }
    }

    /*!
     * @brief Get dimension of flat: Dim - size()
     */
    KOKKOS_INLINE_FUNCTION
    std::size_t
    dimension() const noexcept
    {
        return Dim - _size;
    }

    /*!
     * @brief Get size of representation, that is, the number of specified
     * coordinates
     */
    KOKKOS_INLINE_FUNCTION
    std::size_t
    size() const noexcept
    {
        return _size;
    }

    /*!
     * @brief Map flat coordinate to parent space coordinate
     */
    KOKKOS_INLINE_FUNCTION
    std::size_t
    expandCoordinate(std::size_t i) const
    {
        KOKKOS_ARRAY_BOUNDS_CHECK(i, _size);
        return _idMap[i];
    }

    /*!
     * @brief Convert flat representation to parent space vector (with wildcard
     * coordinate values as appropriate)
     */
    KOKKOS_INLINE_FUNCTION
    VectorType
    expand() const
    {
        auto ret = VectorType::filled(wildcard<ScalarType>);
        for (std::size_t i = 0; i < _size; ++i) {
            ret[expandCoordinate(i)] = _rep[i];
        }
        return ret;
    }

    //!@{
    /*!
     * @brief Subscript operator for accessing elements of the compact
     * representation
     *
     * @warning Index should be less than dimension(), which is usually not the
     * same as Dim
     */
    KOKKOS_INLINE_FUNCTION
    auto
    operator[](std::size_t i) const
    {
        KOKKOS_ARRAY_BOUNDS_CHECK(i, _size);
        return _rep[i];
    }

    KOKKOS_INLINE_FUNCTION
    auto&
    operator[](std::size_t i)
    {
        KOKKOS_ARRAY_BOUNDS_CHECK(i, _size);
        return _rep[i];
    }
    //!@}

private:
    //! Internal representation
    VectorType _rep;
    //! Map of flat indices to parent space indices
    Kokkos::Array<std::size_t, Dim> _idMap;
    //! Size of internal representation
    std::size_t _size;
};


/*!
 * @relates CompactFlat
 * @brief Subtract two CompactFlat objects
 */
template <typename T, std::size_t N>
KOKKOS_INLINE_FUNCTION
CompactFlat<T, N>
operator-(const CompactFlat<T, N>& b, const CompactFlat<T, N>& a)
{
    assert(a.size() == b.size());
    CompactFlat<T, N> ret{b};
    for (std::size_t i = 0; i < a.size(); ++i) {
        ret[i] -= a[i];
    }
    return ret;
}


namespace detail
{
//! @cond
template <typename TScalar, std::size_t Dim>
struct DifferenceTypeHelper<::plsm::CompactFlat<TScalar, Dim>>
{
    using Type = ::plsm::CompactFlat<DifferenceType<TScalar>, Dim>;
};
//! @endcond
}
}
