#pragma once

#include <plsm/IntervalRange.h>

namespace plsm
{
template <typename TScalar, std::size_t Dim>
KOKKOS_INLINE_FUNCTION
SpaceVector<double, Dim>
Region<TScalar, Dim>::dispersion() const noexcept
{
    SpaceVector<double, Dim> disp{};
    for (std::size_t axis = 0; axis < Dim; ++axis) {
        double nSqSum {};
        auto ival = (*this)[axis];
        for (auto n : makeIntervalRange(ival)) {
            nSqSum += static_cast<double>(n*n);
        }
        auto nAvg = ival.midpoint();
        auto factor = 1.0 / static_cast<double>(ival.length());
        disp[axis] = (factor*nSqSum - nAvg*nAvg);
        // Convenient for computing coefficients
        if (ival.length() == 1) disp[axis] = 1.0;
    }

    return disp;
}


template <typename TScalar, std::size_t Dim>
KOKKOS_INLINE_FUNCTION
bool
Region<TScalar, Dim>::contains(const VectorType& p) const
{
    bool ret = true;
    for (std::size_t i = 0; i < Dim; ++i) {
        if (!(*this)[i].contains(p[i])) {
            ret = false;
            break;
        }
    }
    return ret;
}


template <typename TScalar, std::size_t Dim>
KOKKOS_INLINE_FUNCTION
bool
Region<TScalar, Dim>::intersects(const FlatType& p) const
{
    bool ret = true;
    for (std::size_t i = 0; i < p.size(); ++i) {
        if (!(*this)[p.expandCoordinate(i)].contains(p[i])) {
            ret = false;
            break;
        }
    }
    return ret;
}


template <typename TScalar, std::size_t Dim>
KOKKOS_INLINE_FUNCTION
bool
Region<TScalar, Dim>::intersects(const FlatType& a, const FlatType& b) const
{
    bool ret = false;
    if (this->intersects(a) || this->intersects(b)) {
        ret = true;
    }
    else {
        Segment<FlatType> segment{a, b};
        for (std::size_t i = 0; i < a.size(); ++i) {
            if (intersectsFace<RangeElem::first>(i, segment) ||
                    intersectsFace<RangeElem::last>(i, segment)) {
                ret = true;
                break;
            }
        }
    }
    return ret;
}


template <typename TScalar, std::size_t Dim>
template <typename TPoint>
KOKKOS_INLINE_FUNCTION
bool
Region<TScalar, Dim>::intersects(Kokkos::View<TPoint*> points) const
{
    bool ret = false;
    if (points.extent(0) == 1) {
        ret = intersects(points[0]);
    }
    else {
        //Assumes each subsequent pair defines a segment
        for (std::size_t i = 0; i < points.extent(0) - 1; ++i) {
            if (intersects(points[i], points[i+1])) {
                ret = true;
                break;
            }
        }
    }
    return ret;
}


template <typename TScalar, std::size_t Dim>
bool
Region<TScalar, Dim>::intersects(const std::vector<VectorType>& points) const
{
    bool ret = false;
    if (points.size() == 1) {
        ret = intersects(points[0]);
    }
    else {
        //Assumes each subsequent pair defines a segment
        for (std::size_t i = 0; i < points.size() - 1; ++i) {
            if (intersects(points[i], points[i+1])) {
                ret = true;
                break;
            }
        }
    }
    return ret;
}


template <typename TScalar, std::size_t Dim>
KOKKOS_INLINE_FUNCTION
bool
Region<TScalar, Dim>::intersects(const Region& other) const
{
    bool ret = true;
    for (std::size_t i = 0; i < Dim; ++i) {
        if (!(*this)[i].intersects(other[i])) {
            ret = false;
            break;
        }
    }
    return ret;
}


template <typename TScalar, std::size_t Dim>
template <RangeElem Elem>
KOKKOS_INLINE_FUNCTION
bool
Region<TScalar, Dim>::intersectsFace(std::size_t flatCoord,
    const Segment<FlatType>& segment) const
{
    bool ret = false;
    if (segment.vector()[flatCoord] != 0) {
        const auto& sOrig = segment.origin();
        const auto& sVec = segment.vector();
        auto spaceCoord = sOrig.expandCoordinate(flatCoord);
        auto faceVal = get<Elem>((*this)[spaceCoord]);
        auto origComp = sOrig[flatCoord];
        auto vecComp = static_cast<double>(sVec[flatCoord]);
        auto t = static_cast<double>(faceVal - origComp) / vecComp;
        if (t >= 0.0 && t <= 1.0) {
            ret = true;
            for (std::size_t i = 0; i < sOrig.size(); ++i) {
                if (i == flatCoord) {
                    continue;
                }
                auto currComp = static_cast<ScalarType>(
                    static_cast<double>(sOrig[i]) +
                    static_cast<double>(sVec[i])*t);
                if (!(*this)[sOrig.expandCoordinate(i)].contains(currComp)) {
                    ret = false;
                    break;
                }
            }
        }
    }
    return ret;
}
}
