#pragma once

#include <cassert>

#include <plsm/detail/SpaceVectorBase.h>

namespace plsm
{
/*!
 * @brief Segment represents a line segment between two points
 *
 * @tparam TPoint Type representing a point
 *
 * @test test_Segment.cpp
 */
template <typename TPoint>
class Segment
{
public:
	//! Type representing a point
	using PointType = TPoint;
	//! Type representing a vector; uses DifferenceType for subtraction
	using VectorType = DifferenceType<PointType>;

	/*!
	 * @brief Construct from two points
	 */
	KOKKOS_INLINE_FUNCTION
	Segment(const PointType& a, const PointType& b) :
		_origin{a},
		_vector{static_cast<VectorType>(b) - static_cast<VectorType>(a)}
	{
	}

	/*!
	 * @brief Get beginning point
	 */
	KOKKOS_INLINE_FUNCTION
	const PointType&
	origin() const
	{
		return _origin;
	}

	/*!
	 * @brief Get vector; origin() + vector() = <end-point>
	 */
	KOKKOS_INLINE_FUNCTION
	const VectorType&
	vector() const
	{
		return _vector;
	}

private:
	//! Beginning point
	PointType _origin;
	//! Segment vector
	VectorType _vector;
};
} // namespace plsm
