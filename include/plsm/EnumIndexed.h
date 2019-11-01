#pragma once

#include <type_traits>

#include <Kokkos_Macros.hpp>

namespace plsm
{
/*!
 * @brief EnumIndexed allows any 'std::array-like' class to be indexed with an
 * enum type
 */
template <typename TArray, typename TEnumIndex = void>
struct EnumIndexed : TArray
{
    using EnumIndex = TEnumIndex;

    using TArray::TArray;

    KOKKOS_INLINE_FUNCTION
    decltype(auto)
    operator[](EnumIndex enumVal)
    {
        return TArray::operator[](static_cast<std::size_t>(enumVal));
    }

    KOKKOS_INLINE_FUNCTION
    decltype(auto)
    operator[](EnumIndex enumVal) const
    {
        return TArray::operator[](static_cast<std::size_t>(enumVal));
    }

    KOKKOS_INLINE_FUNCTION
    decltype(auto)
    operator[](std::size_t i)
    {
        return TArray::operator[](i);
    }

    KOKKOS_INLINE_FUNCTION
    decltype(auto)
    operator[](std::size_t i) const
    {
        return TArray::operator[](i);
    }

    // using TArray::operator[];
};


template <typename TArray>
struct EnumIndexed<TArray, void> : TArray
{
    using TArray::TArray;
};
}
