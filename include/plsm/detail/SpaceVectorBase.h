#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <ostream>

#include <Kokkos_Array.hpp>

#include <plsm/Utility.h>
#include <plsm/detail/KokkosExtension.h>

namespace plsm
{
namespace detail
{
/*!
 * @brief Generic representation and most basic functionality for a Euclidian
 * vector
 *
 * @tparam TScalar Underlying scalar representation
 * @tparam Dim Vector space dimension
 * @tparam TDerived Implementation class
 */
template <typename TScalar, std::size_t Dim, typename TDerived>
class SpaceVectorBase : public Kokkos::Array<TScalar, Dim>
{
public:
	//! Underlying scalar type
	using ScalarType = TScalar;

	using Kokkos::Array<ScalarType, Dim>::Array;

	/*!
	 * @brief Special copy constructor
	 */
	KOKKOS_INLINE_FUNCTION
	SpaceVectorBase(std::initializer_list<ScalarType> ilist)
	{
		assert(ilist.size() == Dim);
		for (std::size_t i = 0; i < Dim; ++i) {
			(*this)[i] = ilist.begin()[i];
		}
	}

	/*!
	 * @brief Special copy constructor
	 */
	template <typename TScalar2, typename TDerived2>
	KOKKOS_INLINE_FUNCTION
	SpaceVectorBase(const SpaceVectorBase<TScalar2, Dim, TDerived2>& other)
	{
		for (std::size_t i = 0; i < Dim; ++i) {
			(*this)[i] = static_cast<ScalarType>(other[i]);
		}
	}

	/*!
	 * @brief Assignment from initializer_list
	 */
	KOKKOS_INLINE_FUNCTION
	SpaceVectorBase&
	operator=(std::initializer_list<ScalarType> ilist)
	{
		assert(ilist.size() == Dim);
		for (std::size_t i = 0; i < Dim; ++i) {
			(*this)[i] = ilist.begin()[i];
		}
	}

	/*!
	 * @brief Check if this vector has a non-zero component for the given axis
	 * and zero components for every other axis
	 */
	KOKKOS_INLINE_FUNCTION
	bool
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

	/*!
	 * @copydoc isOnAxis()
	 */
	template <typename T, std::enable_if_t<std::is_enum<T>::value, int> = 0>
	KOKKOS_INLINE_FUNCTION
	bool
	isOnAxis(T axis) const noexcept
	{
		return isOnAxis(static_cast<std::size_t>(axis));
	}

	/*!
	 * @brief Construct a vector filled with the given value
	 */
	static KOKKOS_INLINE_FUNCTION
	TDerived
	filled(TScalar value)
	{
		TDerived ret;
		for (std::size_t i = 0; i < Dim; ++i) {
			ret[i] = value;
		}
		return ret;
	}

	/*!
	 * @brief Construct a zero vector
	 */
	static KOKKOS_INLINE_FUNCTION
	TDerived
	zero()
	{
		return filled(static_cast<TScalar>(0));
	}
};

/*!
 * @relates SpaceVectorBase
 * @brief Formatted vector output, for example: {x, y}
 */
template <typename T, std::size_t N, typename TDerived>
std::ostream&
operator<<(std::ostream& os, const SpaceVectorBase<T, N, TDerived>& v)
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

//@{
/*!
 * @relates SpaceVectorBase
 * @brief Equality comparison
 */
template <typename T, std::size_t N, typename TDerived>
KOKKOS_INLINE_FUNCTION
bool
operator==(const SpaceVectorBase<T, N, TDerived>& a,
	const SpaceVectorBase<T, N, TDerived>& b)
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

template <typename T, std::size_t N, typename TDerived>
KOKKOS_INLINE_FUNCTION
bool
operator!=(const SpaceVectorBase<T, N, TDerived>& a,
	const SpaceVectorBase<T, N, TDerived>& b)
{
	return !(a == b);
}
//@}

/*!
 * @relates SpaceVectorBase
 * @brief Compute the component-wise difference
 */
template <typename T, std::size_t N, typename TDerived>
KOKKOS_INLINE_FUNCTION
TDerived
operator-(const SpaceVectorBase<T, N, TDerived>& b,
	const SpaceVectorBase<T, N, TDerived>& a)
{
	TDerived ret;
	for (std::size_t i = 0; i < N; ++i) {
		ret[i] = b[i] - a[i];
	}
	return ret;
}
} // namespace detail
} // namespace plsm
