#pragma once

#include <iterator>

#include <plsm/Interval.h>

namespace plsm
{
/*!
 * @brief IntervalRange converts an Interval into an iterable range
 *
 * Interval doesn't behave like a container. Interval::begin() and
 * Interval::end() return values, but for IntervalRange, these functions return
 * an IntervalRange::Iterator. This can therefore be used in C++ range-based for
 * loops.
 *
 * @tparam TLimit Underlying data type for representing interval limits
 *
 * @test test_IntervalRange.cpp
 */
template <typename TLimit>
class IntervalRange
{
public:
    //! Underlying type for representing interval limits
    using LimitType = TLimit;

    /*!
     * @brief Random-access iterator for iterating over values in an Interval
     *
     * This class simply holds a value of LimitType which is modified when the
     * iterator is advanced
     */
    class Iterator
    {
    public:
        //!@{
        /*!
         * @brief Types required for std::iterator_traits
         */
        using difference_type = LimitType;
        using value_type = LimitType;
        using pointer = const LimitType*;
        using reference = const LimitType&;
        using iterator_category = std::random_access_iterator_tag;
        //!@}

        /*!
         * @brief Must be constructed with an initial value
         */
        explicit
        KOKKOS_INLINE_FUNCTION
        constexpr
        Iterator(LimitType value) noexcept
            :
            _value{value}
        {
        }

        /*!
         * @brief Dereference to get the current value
         */
        KOKKOS_INLINE_FUNCTION
        constexpr reference
        operator*() const noexcept
        {
            return _value;
        }

        /*!
         * @brief (Pre) Increment
         */
        KOKKOS_INLINE_FUNCTION
        Iterator&
        operator++() noexcept
        {
            ++_value;
            return *this;
        }

        /*!
         * @brief (Post) Increment
         */
        KOKKOS_INLINE_FUNCTION
        Iterator
        operator++(int) noexcept
        {
            Iterator ret(_value);
            ++_value;
            return ret;
        }

        /*!
         * @brief Advance by specified incr
         */
        KOKKOS_INLINE_FUNCTION
        Iterator&
        operator+=(difference_type incr) noexcept
        {
            _value += incr;
            return *this;
        }

        /*!
         * @brief Decrement by specified decr value
         */
        KOKKOS_INLINE_FUNCTION
        Iterator&
        operator-=(difference_type decr) noexcept
        {
            _value -= decr;
            return *this;
        }

        /*!
         * @brief Check for equality with another Iterator
         */
        KOKKOS_INLINE_FUNCTION
        constexpr bool
        operator==(Iterator other) const noexcept
        {
            return _value == other._value;
        }

        /*!
         * @brief Check for inequality with another Iterator
         */
        KOKKOS_INLINE_FUNCTION
        constexpr bool
        operator!=(Iterator other) const noexcept
        {
            return !((*this) == other);
        }

    private:
        /*!
         * @brief Add two iterators
         */
        friend
        KOKKOS_INLINE_FUNCTION
        constexpr Iterator
        operator+(Iterator a, Iterator b)
        {
            return Iterator{a._value + b._value};
        }

        /*!
         * @brief Compute distance from Iterator a to Iterator b
         */
        friend
        KOKKOS_INLINE_FUNCTION
        constexpr difference_type
        operator-(Iterator b, Iterator a) noexcept
        {
            return b._value - a._value;
        }

    private:
        //! Current value
        LimitType _value;
    };

    /*!
     * @brief Default construct with empty Interval
     */
    KOKKOS_INLINE_FUNCTION
    constexpr
    IntervalRange() noexcept
        :
        _ival{}
    {
    }

    /*!
     * @brief Construct with Interval [0, end)
     */
    explicit
    KOKKOS_INLINE_FUNCTION
    constexpr
    IntervalRange(LimitType end) noexcept
        :
        _ival{end}
    {
    }

    /*!
     * @brief Construct with Interval [begin, end)
     */
    KOKKOS_INLINE_FUNCTION
    constexpr
    IntervalRange(LimitType begin, LimitType end) noexcept
        :
        _ival{begin, end}
    {
    }

    /*!
     * @brief Construct with existing Interval
     */
    KOKKOS_INLINE_FUNCTION
    constexpr
    IntervalRange(const Interval<LimitType>& ival) noexcept
        :
        _ival{ival}
    {
    }

    /*!
     * @brief Get Iterator to first value
     */
    KOKKOS_INLINE_FUNCTION
    constexpr Iterator
    begin() const noexcept
    {
        return Iterator{_ival.begin()};
    }

    /*!
     * @brief Get Iterator to one-past-the-last value
     */
    KOKKOS_INLINE_FUNCTION
    constexpr Iterator
    end() const noexcept
    {
        return Iterator{_ival.end()};
    }

    /*!
     * @brief Is the Interval empty
     */
    KOKKOS_INLINE_FUNCTION
    constexpr bool
    empty() const noexcept
    {
        return _ival.empty();
    }

    /*!
     * @brief Get the length of the Interval
     */
    KOKKOS_INLINE_FUNCTION
    constexpr typename Interval<LimitType>::SizeType
    size() const noexcept
    {
        return _ival.length();
    }

    /*!
     * @brief Get the underlying Interval
     */
    KOKKOS_INLINE_FUNCTION
    constexpr const Interval<LimitType>&
    interval() const noexcept
    {
        return _ival;
    }

private:
    //! Interval represented
    const Interval<LimitType> _ival;
};


/*!
 * @relates IntervalRange
 * @brief Check for equality between two IntervalRanges
 */
template <typename TLimit>
KOKKOS_INLINE_FUNCTION
constexpr bool
operator==(const IntervalRange<TLimit>& a, const IntervalRange<TLimit>& b)
    noexcept
{
    return a.interval() == b.interval();
}


//!@{
/*!
 * @brief Template helper functions for constructing an IntervalRange
 */
template <typename TLimit>
KOKKOS_INLINE_FUNCTION
constexpr IntervalRange<TLimit>
makeIntervalRange(TLimit end) noexcept
{
    return IntervalRange<TLimit>{end};
}


template <typename TLimit>
KOKKOS_INLINE_FUNCTION
constexpr IntervalRange<TLimit>
makeIntervalRange(TLimit begin, TLimit end) noexcept
{
    return IntervalRange<TLimit>{begin, end};
}


template <typename TLimit>
KOKKOS_INLINE_FUNCTION
constexpr IntervalRange<TLimit>
makeIntervalRange(const Interval<TLimit>& ival) noexcept
{
    return IntervalRange<TLimit>{ival.begin(), ival.end()};
}
//!@}
}
