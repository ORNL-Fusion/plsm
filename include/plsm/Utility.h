#pragma once

//std
#include <limits>

namespace plsm
{
template <typename T>
constexpr T invalid = std::numeric_limits<T>::max() - static_cast<T>(1);


template <typename T>
constexpr T wildcard = std::numeric_limits<T>::max();


namespace detail
{
template <typename T>
struct DifferenceTypeHelper
{
    static_assert(std::is_arithmetic<T>::value,
        "Maybe you need a specialization "
        "(see DifferenceTypeHelper<SpaceVector>)");

    using Type =
        std::conditional_t<std::is_integral<T>::value,
            std::make_signed_t<T>,
            std::conditional_t<std::is_floating_point<T>::value,
                T,
                void>>;
};
}


template <typename T>
using DifferenceType = typename detail::DifferenceTypeHelper<T>::Type;
}
