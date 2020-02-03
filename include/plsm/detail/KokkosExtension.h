#pragma once

#include <cstddef>

#include <Kokkos_Array.hpp>

/*! @file */

namespace Kokkos
{
//@{
/*!
 * @brief Provide begin/end pairs so that Kokkos::Array can be used in a
 * range-based for loop
 */
template <typename T, std::size_t N, typename P>
KOKKOS_INLINE_FUNCTION
auto
begin(Array<T, N, P>& a) noexcept
{
    return a.data();
}


template <typename T, std::size_t N, typename P>
KOKKOS_INLINE_FUNCTION
auto
end(Array<T, N, P>& a) noexcept
{
    return a.data() + static_cast<std::ptrdiff_t>(N);
}


template <typename T, std::size_t N, typename P>
KOKKOS_INLINE_FUNCTION
auto
begin(const Array<T, N, P>& a) noexcept
{
    return a.data();
}


template <typename T, std::size_t N, typename P>
KOKKOS_INLINE_FUNCTION
auto
end(const Array<T, N, P>& a) noexcept
{
    return a.data() + static_cast<std::ptrdiff_t>(N);
}
//@}
}
