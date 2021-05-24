#pragma once

#include <plsm/IntervalRange.h>
#include <plsm/detail/SpaceVectorBase.h>

namespace plsm
{
/*!
 * @brief MultiIndex represents an N-dimensional set of indices
 *
 * This might be used to represent a position within a lattice region
 *
 * @tparam Dim Dimension of the space to be indexed
 *
 * @test test_MultiIndex.cpp
 */
template <DimType Dim>
class MultiIndex : public detail::SpaceVectorBase<IdType, Dim, MultiIndex<Dim>>
{
public:
	//! Alias to parent class type
	using Superclass = detail::SpaceVectorBase<IdType, Dim, MultiIndex<Dim>>;

	using Superclass::Superclass;

	/*!
	 * @brief Get product of all indices
	 *
	 * This is helpful for getting the linear size from a multi-index size
	 */
	KOKKOS_INLINE_FUNCTION
	IdType
	getProduct() const
	{
		IdType ret = 1;
		for (auto i : makeIntervalRange(Dim)) {
			ret *= (*this)[i];
		}
		return ret;
	}
};
} // namespace plsm
