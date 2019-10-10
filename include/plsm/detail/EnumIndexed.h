#pragma once

//std
#include <type_traits>
//Kokkos
#include <Kokkos_Macros.hpp>

namespace plsm
{
namespace detail
{
/*!
 * @brief EnumIndexed allows any 'std::array-like' class to be indexed with an
 * enum type
 *
 * @todo With C++17 we can legally replace the 'class' keyword with 'typename'
 * in the template template parameter TArrayTpl
 */
template <typename TArray, typename TEnumIndex = void>
struct EnumIndexed : TArray
{
    static_assert(std::is_enum<TEnumIndex>::value, "Must use enum type");
    using EnumIndex = TEnumIndex;
    using IndexType = std::underlying_type_t<EnumIndex>;

    using TArray::TArray;
    using TArray::operator[];

    KOKKOS_INLINE_FUNCTION
    decltype(auto)
    operator[](EnumIndex enumVal)
    {
        return (*this)[static_cast<IndexType>(enumVal)];
    }

    KOKKOS_INLINE_FUNCTION
    decltype(auto)
    operator[](EnumIndex enumVal) const
    {
        return (*this)[static_cast<IndexType>(enumVal)];
    }
};


template <typename TArray>
struct EnumIndexed<TArray, void> : TArray
{
    using TArray::TArray;
};
}
}
