#pragma once

#include <Kokkos_Macros.hpp>

#include <plsm/Utility.h>

namespace plsm
{
struct Intersect
{
};


struct Select
{
};


struct SelectAll
{
};


namespace detail
{
template <typename TRefineTag>
struct RefineTraits
{
};


template <>
struct RefineTraits<::plsm::Intersect>
{
    using RefineTag = ::plsm::Intersect;
    using SelectTag = ::plsm::Select;
};


template <>
struct RefineTraits<::plsm::Select>
{
    using RefineTag = ::plsm::Select;
    using SelectTag = ::plsm::SelectAll;
};


template <>
struct RefineTraits<::plsm::SelectAll>
{
    using RefineTag = ::plsm::Intersect;
    using SelectTag = ::plsm::SelectAll;
};
}


namespace refine
{
/*!
 * @brief Detector
 *
 * @todo Need to support 'overlap' (see BallDetector)
 */
template <typename TDerived, typename TRefineTag = ::plsm::Intersect>
class Detector
{
public:
    using Traits = ::plsm::detail::RefineTraits<TRefineTag>;
    using RefineTag = typename Traits::RefineTag;
    using SelectTag = typename Traits::SelectTag;
    static constexpr RefineTag refineTag{};
    static constexpr SelectTag selectTag{};

    static constexpr std::size_t fullDepth = wildcard<std::size_t>;

    Detector() = default;

    Detector(std::size_t refineDepth)
        :
        _depth{refineDepth}
    {
    }

    KOKKOS_INLINE_FUNCTION
    std::size_t
    depth() const noexcept
    {
        return _depth;
    }

    template <typename T>
    KOKKOS_INLINE_FUNCTION
    bool
    operator()(Intersect, T&& region) const
    {
        return static_cast<const TDerived*>(this)->intersect(region);
    }

    template <typename T>
    KOKKOS_INLINE_FUNCTION
    bool
    operator()(Select, T&& region) const
    {
        return static_cast<const TDerived*>(this)->select(region);
    }

    template <typename T>
    KOKKOS_INLINE_FUNCTION
    bool
    operator()(SelectAll, T&&) const
    {
        return true;
    }

protected:
    std::size_t _depth{fullDepth};
};
}
}
