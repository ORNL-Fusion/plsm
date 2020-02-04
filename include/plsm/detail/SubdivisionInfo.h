#pragma once

#include <plsm/MultiIndex.h>

namespace plsm
{
/*!
 * @brief Set of ratios specifying how many subdivisions to make in each
 * dimension
 */
template <std::size_t Dim>
using SubdivisionRatio = MultiIndex<Dim>;

namespace detail
{
/*!
 * @brief A tool to handle arbitrary subdivision ratios at each refinement level
 *
 * This class represents subdivision information for a single refinement level.
 * It stores the subdivision ratio in each direction and performs conversion
 * between linear and multi-indices within the grid defined according to those
 * ratios.
 */
template <std::size_t Dim>
class SubdivisionInfo
{
public:
    SubdivisionInfo() noexcept
        = default;

    /*!
     * @brief Construct from subdivision ratio
     */
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

    /*!
     * @brief Get the subdivision ratio
     */
    KOKKOS_INLINE_FUNCTION
    const SubdivisionRatio<Dim>&
    getRatio() const noexcept
    {
        return _ratio;
    }

    /*!
     * @brief Convert a multi-index to a linear index for the grid defined by
     * the subdivision ratio
     */
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

    /*!
     * @brief Convert a linear index to its corresponding multi-index within the
     * grid defined by the subdivision ratio
     */
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
    //! The subdivision (multi-)ratio fot the current refinement level
    SubdivisionRatio<Dim> _ratio;
    //! Set of strides to ease conversion between linear and multi-indices
    MultiIndex<Dim> _sliceSize;
};
}
}
