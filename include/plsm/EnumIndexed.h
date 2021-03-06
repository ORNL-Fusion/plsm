#pragma once

#include <type_traits>

#include <Kokkos_Macros.hpp>

#include <plsm/Utility.h>

namespace plsm
{
/*!
 * @brief EnumIndexed allows any 'std::array-like' class to be indexed with an
 * enum type
 *
 * @test unittest_EnumIndexed.cpp
 */
template <typename TArray, typename TEnumIndex = void>
struct EnumIndexed : TArray
{
	//! Type of enum index
	using EnumIndex = TEnumIndex;

	using TArray::TArray;

	EnumIndexed() noexcept(noexcept(TArray())) = default;

	/*!
	 * @brief Pass-through copy constructor for underlying container
	 */
	KOKKOS_INLINE_FUNCTION
	EnumIndexed(const TArray& a) : TArray(a)
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

/*!
 * @brief Specialization for when no enum type is provided
 */
template <typename TArray>
struct EnumIndexed<TArray, void> : TArray
{
	using TArray::TArray;
};
} // namespace plsm
