#pragma once

//plsm
#include <plsm/detail/SpaceVectorBase.h>
#include <plsm/IntervalRange.h>

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
template <std::size_t Dim>
class MultiIndex :
    public detail::SpaceVectorBase<std::size_t, Dim, MultiIndex<Dim>>
{
public:
    //! Alias to parent class type
    using Superclass =
        detail::SpaceVectorBase<std::size_t, Dim, MultiIndex<Dim>>;

    using Superclass::Superclass;

    /*!
     * @brief Get product of all indices
     *
     * This is helpful for getting the linear size from a multi-index size
     */
    KOKKOS_INLINE_FUNCTION
    std::size_t
    getProduct() const
    {
        std::size_t ret = 1;
        for (auto i : makeIntervalRange(Dim)) {
            ret *= (*this)[i];
        }
        return ret;
    }
};
}
