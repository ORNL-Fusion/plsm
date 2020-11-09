#pragma once

#include <type_traits>

#include <plsm/detail/SpaceVectorBase.h>

namespace plsm
{
/*!
 * @brief SpaceVector represents an ordered N-tuple
 *
 * A lattice is defined over a vector space. This class is indended to represent
 * a vector in the underlying vector space and can also represent a point in the
 * corresponding (isomorphic) Cartesian product space
 *
 * @tparam TScalar Underlying scalar representation
 * @tparam Dim Dimension of the space
 *
 * @note At this point, SpaceVector adds no functionality to
 * detail::SpaceVectorBase
 *
 * @test test_SpaceVector.cpp
 */
template <typename TScalar, std::size_t Dim>
class SpaceVector :
	public detail::SpaceVectorBase<TScalar, Dim, SpaceVector<TScalar, Dim>>
{
public:
	//! Alias to parent class type
	using Superclass =
		detail::SpaceVectorBase<TScalar, Dim, SpaceVector<TScalar, Dim>>;

	using Superclass::Superclass;
};

namespace detail
{
//! @cond
template <typename TScalar, std::size_t Dim>
struct DifferenceTypeHelper<::plsm::SpaceVector<TScalar, Dim>>
{
	using Type = ::plsm::SpaceVector<DifferenceType<TScalar>, Dim>;
};
//! @endcond
} // namespace detail
} // namespace plsm
