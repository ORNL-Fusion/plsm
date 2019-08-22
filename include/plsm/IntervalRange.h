#pragma once

//std
#include <iterator>
//plsm
#include <plsm/Interval.h>

namespace plsm
{
template <typename TLimit>
class IntervalRange
{
public:
    using LimitType = TLimit;

    class Iterator
    {
    public:
        using difference_type = LimitType;
        using value_type = LimitType;
        using pointer = const LimitType*;
        using reference = const LimitType&;
        using iterator_category = std::random_access_iterator_tag;

        explicit
        KOKKOS_INLINE_FUNCTION
        constexpr
        Iterator(LimitType value) noexcept
            :
            _value{value}
        {
        }

        KOKKOS_INLINE_FUNCTION
        constexpr reference
        operator*() const noexcept
        {
            return _value;
        }

        KOKKOS_INLINE_FUNCTION
        Iterator&
        operator++() noexcept
        {
            ++_value;
            return *this;
        }

        KOKKOS_INLINE_FUNCTION
        Iterator
        operator++(int) noexcept
        {
            Iterator ret(_value);
            ++_value;
            return ret;
        }

        KOKKOS_INLINE_FUNCTION
        Iterator&
        operator+=(difference_type incr) noexcept
        {
            _value += incr;
            return *this;
        }

        KOKKOS_INLINE_FUNCTION
        Iterator&
        operator-=(difference_type decr) noexcept
        {
            _value -= decr;
            return *this;
        }

        KOKKOS_INLINE_FUNCTION
        constexpr bool
        operator==(Iterator other) const noexcept
        {
            return _value == other._value;
        }

        KOKKOS_INLINE_FUNCTION
        constexpr bool
        operator!=(Iterator other) const noexcept
        {
            return !((*this) == other);
        }

    private:
        friend
        KOKKOS_INLINE_FUNCTION
        constexpr difference_type
        operator-(Iterator b, Iterator a) noexcept
        {
            return b._value - a._value;
        }

    private:
        LimitType _value;
    };

    KOKKOS_INLINE_FUNCTION
    constexpr
    IntervalRange() noexcept
        :
        _ival{}
    {
    }

    explicit
    KOKKOS_INLINE_FUNCTION
    constexpr
    IntervalRange(LimitType end) noexcept
        :
        _ival{end}
    {
    }

    KOKKOS_INLINE_FUNCTION
    constexpr
    IntervalRange(LimitType begin, LimitType end) noexcept
        :
        _ival{begin, end}
    {
    }

    KOKKOS_INLINE_FUNCTION
    constexpr
    IntervalRange(const Interval<LimitType>& ival) noexcept
        :
        _ival{ival}
    {
    }

    KOKKOS_INLINE_FUNCTION
    constexpr Iterator
    begin() const noexcept
    {
        return Iterator{_ival.begin()};
    }

    KOKKOS_INLINE_FUNCTION
    constexpr Iterator
    end() const noexcept
    {
        return Iterator{_ival.end()};
    }

    KOKKOS_INLINE_FUNCTION
    constexpr const Interval<LimitType>&
    interval() const noexcept
    {
        return _ival;
    }

private:
    const Interval<LimitType> _ival;
};


template <typename TLimit>
KOKKOS_INLINE_FUNCTION
constexpr bool
operator==(const IntervalRange<TLimit>& a, const IntervalRange<TLimit>& b)
    noexcept
{
    return a.interval() == b.interval();
}


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
}
