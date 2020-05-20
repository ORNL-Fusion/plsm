#pragma once

#include <Kokkos_Macros.hpp>

#include <plsm/Utility.h>

namespace plsm
{
namespace refine
{
struct Refine
{
};

struct Intersect
{
};

struct Overlap
{
};

struct Select
{
};

struct SelectAll
{
};

struct IntersectAndOverlap
{
};

namespace detail
{
template <typename TRefineTag = void>
struct RefineTraits
{
};

template <>
struct RefineTraits<void>
{
    using RefineTag = ::plsm::refine::Refine;
    using SelectTag = ::plsm::refine::Select;
};

template <typename TRefine, typename TSelect>
struct RefineTraits<std::tuple<TRefine, TSelect>>
{
    using RefineTag = TRefine;
    using SelectTag = TSelect;
};

template <>
struct RefineTraits<::plsm::refine::IntersectAndOverlap>
{
    using RefineTag = ::plsm::refine::Intersect;
    using SelectTag = ::plsm::refine::Overlap;
};

template <>
struct RefineTraits<::plsm::refine::Intersect>
{
    using RefineTag = ::plsm::refine::Intersect;
    using SelectTag = ::plsm::refine::Select;
};

template <>
struct RefineTraits<::plsm::refine::Overlap>
{
    using RefineTag = ::plsm::refine::Overlap;
    using SelectTag = ::plsm::refine::Select;
};
}

/*!
 * @brief Detector
 *
 * @todo Need to support 'overlap' (see BallDetector)
 */
template <typename TDerived, typename TRefineTag = void>
class Detector
{
public:
    using Traits = ::plsm::refine::detail::RefineTraits<TRefineTag>;
    using RefineTag = typename Traits::RefineTag;
    using SelectTag = typename Traits::SelectTag;

    template <typename TRegion>
    using BoolVec = Kokkos::Array<bool, TRegion::dimension()>;

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

    template <typename... TArgs>
    KOKKOS_INLINE_FUNCTION
    bool
    operator()(::plsm::refine::Refine, TArgs&&... args) const
    {
        return asDerived()->refine(std::forward<TArgs>(args)...);
    }

    template <typename... TArgs>
    KOKKOS_INLINE_FUNCTION
    bool
    operator()(::plsm::refine::Intersect, TArgs&&... args) const
    {
        return asDerived()->intersect(std::forward<TArgs>(args)...);
    }

    template <typename... TArgs>
    KOKKOS_INLINE_FUNCTION
    bool
    operator()(::plsm::refine::Overlap, TArgs&&... args) const
    {
        return asDerived()->overlap(std::forward<TArgs>(args)...);
    }

    template <typename... TArgs>
    KOKKOS_INLINE_FUNCTION
    bool
    operator()(::plsm::refine::Select, TArgs&&... args) const
    {
        return asDerived()->select(std::forward<TArgs>(args)...);
    }

    template <typename... TArgs>
    KOKKOS_INLINE_FUNCTION
    bool
    operator()(::plsm::refine::SelectAll, TArgs&&...) const
    {
        return true;
    }

    template <typename TBoolVec>
    KOKKOS_INLINE_FUNCTION
    bool
    applyResult(bool val, TBoolVec& result) const
    {
        for (std::size_t i = 0; i < result.size(); ++i) {
            result[i] = val;
        }
        return val;
    }

    template <typename TRegion, typename TBoolVec = BoolVec<TRegion>>
    KOKKOS_INLINE_FUNCTION
    bool
    refine(TRegion&& region, TBoolVec& result) const
    {
        return applyResult(asDerived()->refine(region), result);
    }

    template <typename TRegion, typename TBoolVec = BoolVec<TRegion>>
    KOKKOS_INLINE_FUNCTION
    bool
    intersect(TRegion&& region, TBoolVec& result) const
    {
        return applyResult(asDerived()->intersect(region), result);
    }

    template <typename TRegion, typename TBoolVec = BoolVec<TRegion>>
    KOKKOS_INLINE_FUNCTION
    bool
    overlap(TRegion&& region, TBoolVec& result) const
    {
        return applyResult(asDerived()->overlap(region), result);
    }

protected:
    const TDerived*
    asDerived() const noexcept
    {
        return static_cast<const TDerived*>(this);
    }

protected:
    std::size_t _depth{fullDepth};
};
}
}
