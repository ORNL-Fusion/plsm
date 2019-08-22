#pragma once

//std
#include <cassert>
#include <iostream>
#include <type_traits>
//Kokkos
#include <Kokkos_Macros.hpp>

namespace plsm
{
enum class RangeElem
{
    first,
    last
};


//NOTE: Requires that begin <= end
template <typename TLimit>
class Interval
{
    static_assert(std::is_integral<TLimit>::value,
        "plsm::Interval: template parameter must be an integral type");

public:
    using LimitType = TLimit;
    using SizeType = std::make_unsigned_t<LimitType>;

    KOKKOS_INLINE_FUNCTION
    constexpr
    Interval() noexcept
        = default;

    explicit
    KOKKOS_INLINE_FUNCTION
    constexpr
    Interval(LimitType end) noexcept
        :
        _end{end}
    {
        assert(end >= LimitType{});
    }

    KOKKOS_INLINE_FUNCTION
    constexpr
    Interval(LimitType begin, LimitType end) noexcept
        :
        _begin{begin},
        _end{end}
    {
        assert(begin <= end);
    }

    KOKKOS_INLINE_FUNCTION
    constexpr
    Interval(const Interval&) noexcept
        = default;

    KOKKOS_INLINE_FUNCTION
    Interval&
    operator=(const Interval&) noexcept
        = default;

    KOKKOS_INLINE_FUNCTION
    constexpr LimitType
    begin() const noexcept
    {
        return _begin;
    }

    KOKKOS_INLINE_FUNCTION
    constexpr LimitType
    end() const noexcept
    {
        return _end;
    }

    KOKKOS_INLINE_FUNCTION
    constexpr bool
    empty() const noexcept
    {
        return _begin == _end;
    }

    KOKKOS_INLINE_FUNCTION
    constexpr SizeType
    length() const noexcept
    {
        return static_cast<SizeType>(_end - _begin);
    }

    KOKKOS_INLINE_FUNCTION
    constexpr bool
    contains(LimitType value) const noexcept
    {
        return (_begin <= value) && (value < _end);
    }

    KOKKOS_INLINE_FUNCTION
    constexpr bool
    intersects(const Interval& other) const noexcept
    {
        if (empty() || other.empty()) {
            return false;
        }
        if (other._begin < _begin && (other._end - 1) < _begin) {
            return false;
        }
        if (_begin < other._begin && (_end - 1) < other._begin) {
            return false;
        }
        return true;
    }

private:
    LimitType _begin{};
    LimitType _end{};
};


template <typename T, typename U>
KOKKOS_INLINE_FUNCTION
constexpr bool
operator==(const Interval<T>& a, const Interval<U> b) noexcept
{
    return (a.begin() == b.begin()) && (a.end() == b.end());
}


template <typename T, typename U>
KOKKOS_INLINE_FUNCTION
constexpr bool
operator!=(const Interval<T>& a, const Interval<U> b) noexcept
{
    return !(a == b);
}


template <typename T>
inline
std::ostream&
operator<<(std::ostream& os, const Interval<T>& x)
{
    os << '[' << begin(x) << ',' << end(x) << ')';
    return os;
}


template <typename T>
KOKKOS_INLINE_FUNCTION
constexpr T
begin(const Interval<T>& x) noexcept
{
    return x.begin();
}


template <typename T>
KOKKOS_INLINE_FUNCTION
constexpr T
end(const Interval<T>& x) noexcept
{
    return x.end();
}


namespace detail
{
template <RangeElem Elem>
struct GetRangeElemHelper
{
};


template <>
struct GetRangeElemHelper<RangeElem::first>
{
    template <typename T>
    KOKKOS_INLINE_FUNCTION
    constexpr T
    operator()(const Interval<T>& x) noexcept
    {
        return x.begin();
    }
};


template <>
struct GetRangeElemHelper<RangeElem::last>
{
    template <typename T>
    KOKKOS_INLINE_FUNCTION
    constexpr T
    operator()(const Interval<T>& x) noexcept
    {
        return x.end() - static_cast<T>(1);
    }
};
} /* namespace detail */


template <RangeElem Elem, typename TLimit>
KOKKOS_INLINE_FUNCTION
constexpr TLimit
get(const Interval<TLimit>& x) noexcept
{
    return detail::GetRangeElemHelper<Elem>{}(x);
}
} /* namespace plsm */
