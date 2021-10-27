#pragma once

#include <type_traits>

#include <Kokkos_Macros.hpp>

#include <plsm/Utility.h>

namespace plsm
{
/*!
 * @brief Allows any 'std::array-like' class to be indexed with an enum type
 *
 * @test unittest_EnumIndexed.cpp
 */
template <typename TArray, typename TEnumIndex>
struct IndexWithEnum : TArray
{
	//! Type of enum index
	using EnumIndex = TEnumIndex;

	using TArray::TArray;

	IndexWithEnum() noexcept(noexcept(TArray())) = default;

	/*!
	 * @brief Pass-through copy constructor for underlying container
	 */
	KOKKOS_INLINE_FUNCTION
	IndexWithEnum(const TArray& a) : TArray(a)
	{
	}

	//!@{
	/*!
	 * @brief Indexing operators; separate integer and enum versions because
	 * Kokkos::Array allows enums without handling scoped enums
	 */
	KOKKOS_INLINE_FUNCTION
	decltype(auto)
	operator[](EnumIndex enumVal)
	{
		return TArray::operator[](static_cast<DimType>(enumVal));
	}

	KOKKOS_INLINE_FUNCTION
	decltype(auto)
	operator[](EnumIndex enumVal) const
	{
		return TArray::operator[](static_cast<DimType>(enumVal));
	}

	KOKKOS_INLINE_FUNCTION
	decltype(auto)
	operator[](DimType i)
	{
		return TArray::operator[](i);
	}

	KOKKOS_INLINE_FUNCTION
	decltype(auto)
	operator[](DimType i) const
	{
		return TArray::operator[](i);
	}
	//!@}
};

namespace detail
{
template <typename TArray, typename TEnumIndex = void>
struct EnumIndexedHelper
{
	using Type = IndexWithEnum<TArray, TEnumIndex>;
};

template <typename TArray>
struct EnumIndexedHelper<TArray, void>
{
	using Type = TArray;
};
} // namespace detail

/*!
 * @brief Allows any 'std::array-like' class to be indexed with an enum type
 *
 * Defaults to alias to `TArray` (when `void` is used). When an enum type
 * template argument is used, this aliases to an instantiation of IndexWithEnum.
 * See detail::EnumIndexedHelper definition(s).
 */
template <typename TArray, typename TEnumIndex = void>
using EnumIndexed =
	typename detail::EnumIndexedHelper<TArray, TEnumIndex>::Type;
} // namespace plsm
