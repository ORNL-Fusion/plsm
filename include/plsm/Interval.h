#pragma once

/** @file */

//std
#include <cassert>
#include <iostream>
#include <type_traits>
//Kokkos
#include <Kokkos_Macros.hpp>

/*!
 * @brief plsm
 */
namespace plsm
{
/*!
 * @enum RangeElem
 * @brief RangeElem describes the inclusive endpoints of a range
 */
enum class RangeElem
{
    first, /*!< The first element (as in front) */
    last   /*!< The last element (inclusive, as in back) */
};


/*!
 * @brief Interval represtents a half-open range of integers
 *
 * Holds two values describing the range [begin, end)
 *
 * @tparam TLimit Underlying data type for representing interval limits
 *
 * @note This class assumes that begin <= end
 * @note Template type parameter TLimit must be an integral type
 *
 * @test test_Interval.cpp
 */
template <typename TLimit>
class Interval
{
    static_assert(std::is_integral<TLimit>::value,
        "plsm::Interval: template parameter must be an integral type");

public:
    //! Underlying type for representing interval limits
    using LimitType = TLimit;
    //! Unsigned type corresponding to LimitType
    using SizeType = std::make_unsigned_t<LimitType>;

    /*!
     * @brief Default construct empty interval [0, 0)
     */
    KOKKOS_INLINE_FUNCTION
    constexpr
    Interval() noexcept
        = default;

    /*!
     * @brief Construct interval [0, end)
     */
    explicit
    KOKKOS_INLINE_FUNCTION
    constexpr
    Interval(LimitType end) noexcept
        :
        _end{end}
    {
        assert(end >= LimitType{});
    }

    /*!
     * @brief Construct interval [begin, end)
     */
    KOKKOS_INLINE_FUNCTION
    constexpr
    Interval(LimitType begin, LimitType end) noexcept
        :
        _begin{begin},
        _end{end}
    {
        assert(begin <= end);
    }

    /*!
     * @brief Defaulted copy operations
     */
    KOKKOS_INLINE_FUNCTION
    constexpr
    Interval(const Interval&) noexcept
        = default;

    /*!
     * @brief Defaulted copy operations
     */
    KOKKOS_INLINE_FUNCTION
    Interval&
    operator=(const Interval&) noexcept
        = default;

    /*!
     * @brief Get the lower limit
     */
    KOKKOS_INLINE_FUNCTION
    constexpr LimitType
    begin() const noexcept
    {
        return _begin;
    }

    /*!
     * @brief Get the upper limit
     */
    KOKKOS_INLINE_FUNCTION
    constexpr LimitType
    end() const noexcept
    {
        return _end;
    }

    /*!
     * @brief Is the Interval empty (begin == end)
     */
    KOKKOS_INLINE_FUNCTION
    constexpr bool
    empty() const noexcept
    {
        return _begin == _end;
    }

    /*!
     * @brief Get the size of the interval
     */
    KOKKOS_INLINE_FUNCTION
    constexpr SizeType
    length() const noexcept
    {
        return static_cast<SizeType>(_end - _begin);
    }

    /*!
     * @brief Check if a scalar value resides within the Interval
     */
    KOKKOS_INLINE_FUNCTION
    constexpr bool
    contains(LimitType value) const noexcept
    {
        return (_begin <= value) && (value < _end);
    }

    /*!
     * @brief Check if this Interval overlaps with another
     * @todo Change name to "overlaps"
     */
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
    //! Lower limit
    LimitType _begin{};
    //! Upper limit
    LimitType _end{};
};


/*!
 * @relates Interval
 * @brief Check equality of two Intervals, potentially using different
 * underlying types
 */
template <typename T, typename U>
KOKKOS_INLINE_FUNCTION
constexpr bool
operator==(const Interval<T>& a, const Interval<U> b) noexcept
{
    return (a.begin() == b.begin()) && (a.end() == b.end());
}


/*!
 * @relates Interval
 * @brief Two Intervals are not equal
 */
template <typename T, typename U>
KOKKOS_INLINE_FUNCTION
constexpr bool
operator!=(const Interval<T>& a, const Interval<U> b) noexcept
{
    return !(a == b);
}


/*!
 * @relates Interval
 * @brief Insert an Interval to an output stream
 */
template <typename T>
inline
std::ostream&
operator<<(std::ostream& os, const Interval<T>& x)
{
    os << '[' << begin(x) << ',' << end(x) << ')';
    return os;
}


/*!
 * @relates Interval
 * @brief Free function to get lower bound of an Interval
 */
template <typename T>
KOKKOS_INLINE_FUNCTION
constexpr T
begin(const Interval<T>& x) noexcept
{
    return x.begin();
}


/*!
 * @relates Interval
 * @brief Free function to get upper bound of an Interval
 */
template <typename T>
KOKKOS_INLINE_FUNCTION
constexpr T
end(const Interval<T>& x) noexcept
{
    return x.end();
}


namespace detail
{
//! @cond
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
//! @endcond
} /* namespace detail */


/*!
 * @relates Interval
 * @brief Get a the lower or upper limit of the given Interval by specifying the
 * desired RangeElem
 */
template <RangeElem Elem, typename TLimit>
KOKKOS_INLINE_FUNCTION
constexpr TLimit
get(const Interval<TLimit>& x) noexcept
{
    return detail::GetRangeElemHelper<Elem>{}(x);
}
} /* namespace plsm */
