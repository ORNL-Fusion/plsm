#pragma once

#include <cassert>
#include <limits>
#include <type_traits>

#include <Kokkos_Macros.hpp>

namespace plsm
{
//! Convention chosen to represent an invalid value for the given type
template <typename T>
inline constexpr T invalid = std::numeric_limits<T>::max() - static_cast<T>(1);

//! Convention chosen to represent a wildcard value for the given type
template <typename T>
inline constexpr T wildcard = std::numeric_limits<T>::max();

template <typename, std::size_t, typename, typename>
class Subpaving;

namespace detail
{
/*!
 * @brief Checks whether T is an instantiation of Subpaving
 */
template <typename T>
struct IsSubpaving;

/*! @cond */
template <typename T>
struct IsSubpaving : std::false_type
{
};

template <typename TScalar, std::size_t Dim, typename TEnumIndex,
	typename TItemData>
struct IsSubpaving<::plsm::Subpaving<TScalar, Dim, TEnumIndex, TItemData>> :
	std::true_type
{
};
/*! @endcond */

/*!
 * @brief Determine the type for the result of a subtraction
 */
template <typename T>
struct DifferenceTypeHelper
{
	static_assert(std::is_arithmetic<T>::value,
		"Maybe you need a specialization "
		"(see DifferenceTypeHelper<SpaceVector>)");

	//! Alias for difference type appropriate for T
	using Type =
		std::conditional_t<std::is_integral_v<T>, std::make_signed_t<T>,
			std::conditional_t<std::is_floating_point_v<T>, T, void>>;
};
} // namespace detail

/*!
 * @brief Determine the type for the result of a subtraction
 */
template <typename T>
using DifferenceType = typename detail::DifferenceTypeHelper<T>::Type;

/*!
 * @brief Specialized assert for requiring a value to be non-negative
 */
template <typename T, std::enable_if_t<std::is_signed<T>::value, int> = 0>
KOKKOS_INLINE_FUNCTION
constexpr void
assertNonNegative(T value)
{
	assert(value >= T{});
}

/*!
 * @brief Specialized assert for requiring a value to be non-negative
 */
template <typename T, std::enable_if_t<std::is_unsigned<T>::value, int> = 0>
KOKKOS_INLINE_FUNCTION
constexpr void assertNonNegative(T)
{
}

/**
 * Duplicate of the generic std::min to be used in device kernels
 */
template <typename T>
KOKKOS_INLINE_FUNCTION
const T&
min(const T& a, const T& b)
{
	if (b < a) {
		return b;
	}
	return a;
}

/**
 * Duplicate of the generic std::max to be used in device kernels
 */
template <typename T>
KOKKOS_INLINE_FUNCTION
const T&
max(const T& a, const T& b)
{
	if (a < b) {
		return b;
	}
	return a;
}

//!@{
/**
 * @brief Duplicate of the generic std::abs to be used in device kernels
 */
template <typename T, std::enable_if_t<std::is_signed<T>::value, int> = 0>
KOKKOS_INLINE_FUNCTION
T
abs(T a)
{
	return max(-a, a);
}

template <typename T, std::enable_if_t<std::is_unsigned<T>::value, int> = 0>
KOKKOS_INLINE_FUNCTION
T
abs(T a)
{
	return a;
}
//!@}
} // namespace plsm
