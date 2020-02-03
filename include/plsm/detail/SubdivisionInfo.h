#pragma once

#include <plsm/MultiIndex.h>

namespace plsm
{
template <std::size_t Dim>
using SubdivisionRatio = MultiIndex<Dim>;

namespace detail
{
template <std::size_t Dim>
class SubdivisionInfo
{
public:
    SubdivisionInfo() noexcept
        = default;

    KOKKOS_INLINE_FUNCTION
    SubdivisionInfo(const SubdivisionRatio<Dim>& ratio)
        :
        _ratio{ratio}
    {
        _sliceSize[Dim-1] = 1;
        for (auto i : makeIntervalRange(Dim - 1)) {
            _sliceSize[Dim-2-i] = _sliceSize[Dim-1-i]*_ratio[Dim-1-i];
        }
    }

    KOKKOS_INLINE_FUNCTION
    const SubdivisionRatio<Dim>&
    getRatio() const noexcept
    {
        return _ratio;
    }

    KOKKOS_INLINE_FUNCTION
    std::size_t
    getLinearIndex(const MultiIndex<Dim>& mId) const
    {
        std::size_t ret = 0;
        for (auto i : makeIntervalRange(Dim)) {
            ret += mId[i]*_sliceSize[i];
        }
        return ret;
    }

    KOKKOS_INLINE_FUNCTION
    MultiIndex<Dim>
    getMultiIndex(std::size_t linearIndex) const
    {
        MultiIndex<Dim> ret;
        for (auto i : makeIntervalRange(Dim)) {
            ret[i] = linearIndex / _sliceSize[i];
            linearIndex = linearIndex % _sliceSize[i];
        }
        return ret;
    }

private:
    SubdivisionRatio<Dim> _ratio;
    MultiIndex<Dim> _sliceSize;
};
}
}
