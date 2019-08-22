#pragma once

//std
#include <type_traits>
//plsm
#include <plsm/detail/SpaceVectorBase.h>

namespace plsm
{
template <typename TScalar, std::size_t Dim>
class SpaceVector :
    public detail::SpaceVectorBase<TScalar, Dim, SpaceVector<TScalar, Dim>>
{
public:
    using Superclass =
        detail::SpaceVectorBase<TScalar, Dim, SpaceVector<TScalar, Dim>>;

    using Superclass::Superclass;
};


namespace detail
{
template <typename TScalar, std::size_t Dim>
struct DifferenceTypeHelper<::plsm::SpaceVector<TScalar, Dim>>
{
    using Type = ::plsm::SpaceVector<DifferenceType<TScalar>, Dim>;
        // std::conditional_t<std::is_integral<TScalar>::value,
        //     ::plsm::SpaceVector<std::make_signed_t<TScalar>, Dim>,
        //     std::conditional_t<std::is_floating_point<TScalar>::value,
        //         ::plsm::SpaceVector<TScalar, Dim>,
        //         void>>;
};
}
}
