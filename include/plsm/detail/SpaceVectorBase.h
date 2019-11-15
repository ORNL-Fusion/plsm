#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>

#include <Kokkos_Array.hpp>

#include <plsm/Utility.h>
#include <plsm/detail/KokkosExtension.h>

namespace plsm
{
namespace detail
{
template <typename TScalar, std::size_t Dim, typename Derived>
class SpaceVectorBase : public Kokkos::Array<TScalar, Dim>
{
public:
    using ScalarType = TScalar;

    using Kokkos::Array<ScalarType, Dim>::Array;

    KOKKOS_INLINE_FUNCTION
    SpaceVectorBase(std::initializer_list<ScalarType> ilist)
    {
        assert(ilist.size() == Dim);
        for (std::size_t i = 0; i < Dim; ++i) {
            (*this)[i] = ilist.begin()[i];
        }
    }

    template <typename TScalar2, typename TDerived2>
    KOKKOS_INLINE_FUNCTION
    SpaceVectorBase(const SpaceVectorBase<TScalar2, Dim, TDerived2>& other)
    {
        for (std::size_t i = 0; i < Dim; ++i) {
            (*this)[i] = static_cast<ScalarType>(other[i]);
        }
    }

    KOKKOS_INLINE_FUNCTION
    SpaceVectorBase&
    operator=(std::initializer_list<ScalarType> ilist)
    {
        assert(ilist.size() == Dim);
        for (std::size_t i = 0; i < Dim; ++i) {
            (*this)[i] = ilist.begin()[i];
        }
    }

    // template <typename T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
    KOKKOS_INLINE_FUNCTION
    bool
    // isOnAxis(T axis) const noexcept
    isOnAxis(std::size_t axis) const noexcept
    {
        constexpr auto zero = static_cast<ScalarType>(0);
        if ((*this)[axis] == zero) {
            return false;
        }
        bool ret = true;
        for (std::size_t i = 0; i < Dim; ++i) {
            if (i == axis) {
                continue;
            }
            if ((*this)[i] != zero) {
                ret = false;
                break;
            }
        }
        return ret;
    }

    template <typename T, std::enable_if_t<std::is_enum<T>::value, int> = 0>
    KOKKOS_INLINE_FUNCTION
    bool
    isOnAxis(T axis) const noexcept
    {
        return isOnAxis(static_cast<std::size_t>(axis));
    }

    static
    KOKKOS_INLINE_FUNCTION
    Derived
    filled(TScalar value)
    {
        Derived ret;
        for (std::size_t i = 0; i < Dim; ++i) {
            ret[i] = value;
        }
        return ret;
    }
};


template <typename T, std::size_t N, typename Derived>
std::ostream&
operator<<(std::ostream& os, const SpaceVectorBase<T, N, Derived>& v)
{
    os << "{";
    if (N > 0) {
        os << v[0];
        for (std::size_t i = 1; i < N; ++i) {
            os << ", " << v[i];
        }
    }
    os << "}";
    return os;
}


template <typename T, std::size_t N, typename Derived>
KOKKOS_INLINE_FUNCTION
bool
operator==(const SpaceVectorBase<T, N, Derived>& a,
    const SpaceVectorBase<T, N, Derived>& b)
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


template <typename T, std::size_t N, typename Derived>
KOKKOS_INLINE_FUNCTION
Derived
operator-(const SpaceVectorBase<T, N, Derived>& b,
    const SpaceVectorBase<T, N, Derived>& a)
{
    Derived ret;
    for (std::size_t i = 0; i < N; ++i) {
        ret[i] = b[i] - a[i];
    }
    return ret;
}
}
}
