#pragma once

//std
#include <cassert>
//plsm
#include <plsm/detail/SpaceVectorBase.h>

namespace plsm
{
template <typename TPoint>
class Segment
{
public:
    using PointType = TPoint;
    using VectorType = DifferenceType<PointType>;

    KOKKOS_INLINE_FUNCTION
    Segment(const PointType& a, const PointType& b)
        :
        _origin{a},
        _vector{static_cast<VectorType>(b) - static_cast<VectorType>(a)}
    {
    }

    KOKKOS_INLINE_FUNCTION
    const PointType&
    origin() const
    {
        return _origin;
    }

    KOKKOS_INLINE_FUNCTION
    const VectorType&
    vector() const
    {
        return _vector;
    }

private:
    PointType _origin;
    VectorType _vector;
};
}
