#pragma once

//plsm
#include <plsm/detail/SpaceVectorBase.h>
#include <plsm/IntervalRange.h>

namespace plsm
{
template <std::size_t Dim>
class MultiIndex :
    public detail::SpaceVectorBase<std::size_t, Dim, MultiIndex<Dim>>
{
public:
    using Superclass =
        detail::SpaceVectorBase<std::size_t, Dim, MultiIndex<Dim>>;

    using Superclass::Superclass;

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


namespace detail
{
template <std::size_t Dim>
struct DifferenceTypeHelper<MultiIndex<Dim>>
{
    // using Type =
};
}
}
