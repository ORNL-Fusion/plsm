#pragma once

//std
#include <algorithm>
#include <cassert>
#include <limits>
//plsm
#include <plsm/SpaceVector.h>
#include <plsm/Utility.h>

namespace plsm
{
template <typename TScalar, std::size_t Dim>
class CompactFlat;

template <typename TScalar, std::size_t Dim>
class CompactFlat
{
    template <typename TScalar2, std::size_t Dim2>
    friend class CompactFlat;

public:
    using ScalarType = TScalar;
    using VectorType = SpaceVector<ScalarType, Dim>;

    KOKKOS_INLINE_FUNCTION
    CompactFlat() noexcept
        :
        _size{0}
    {
    }

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

    KOKKOS_INLINE_FUNCTION
    std::size_t
    dimension() const noexcept
    {
        return Dim - _size;
    }

    KOKKOS_INLINE_FUNCTION
    std::size_t
    size() const noexcept
    {
        return _size;
    }

    KOKKOS_INLINE_FUNCTION
    std::size_t
    expandCoordinate(std::size_t i) const
    {
        KOKKOS_ARRAY_BOUNDS_CHECK(i, _size);
        return _idMap[i];
    }

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

private:
    VectorType _rep;
    Kokkos::Array<std::size_t, Dim> _idMap;
    std::size_t _size;
};


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
template <typename TScalar, std::size_t Dim>
struct DifferenceTypeHelper<::plsm::CompactFlat<TScalar, Dim>>
{
    using Type = ::plsm::CompactFlat<DifferenceType<TScalar>, Dim>;
        // std::conditional_t<std::is_integral<TScalar>::value,
        //     ::plsm::CompactFlat<std::make_signed_t<TScalar>, Dim>,
        //     std::conditional_t<std::is_floating_point<TScalar>::value,
        //         ::plsm::CompactFlat<TScalar, Dim>,
        //         void>>;
};
}
}
