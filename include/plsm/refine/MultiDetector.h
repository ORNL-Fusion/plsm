#pragma once

#include <plsm/refine/Detector.h>

namespace plsm
{
namespace refine
{
namespace detail
{
/*!
 * @brief Default implementation has no actual detector so just acts as starting
 * point for chained logical operations
 */
template <typename... TDetectors>
class MultiDetectorImpl
{
public:
	/*!
	 * @brief Default refine implementation
	 * @return false as starting point for logical disjunction
	 */
	template <typename TRegion, typename TBoolVec>
	KOKKOS_INLINE_FUNCTION
	bool
	refine(const TRegion&, TBoolVec& result) const
	{
		constexpr bool ret = false;
		constexpr std::size_t N = TBoolVec::size();
		for (std::size_t i = 0; i < N; ++i) {
			result[i] = ret;
		}
		return ret;
	}

	/*!
	 * @brief Default select implementation
	 * @return true as starting point for logical conjunction
	 */
	template <typename TRegion>
	KOKKOS_INLINE_FUNCTION
	bool
	select(const TRegion&) const
	{
		return true;
	}
};

/*!
 * @brief Implementation with at least one detector (head) forms chain with
 * instances for remaining detectors (tail)
 */
template <typename TDetector, typename... TDetectors>
class MultiDetectorImpl<TDetector, TDetectors...> :
	private MultiDetectorImpl<TDetectors...>
{
public:
	//! Alias for my detector type
	using Head = TDetector;
	//! Alias for next link in the chain
	using Tail = MultiDetectorImpl<TDetectors...>;

	template <typename TRegion>
	using BoolVec = typename Head::template BoolVec<TRegion>;

	/*!
	 * @brief Construct with detector(s)
	 *
	 * Initialize _detector, pass on the rest to Tail
	 */
	template <typename THead, typename... TTailDetectors>
	MultiDetectorImpl(THead&& detector, TTailDetectors&&... tailDetectors) :
		Tail(std::forward<TTailDetectors>(tailDetectors)...),
		_detector(std::forward<THead>(detector))
	{
	}

	MultiDetectorImpl(const MultiDetectorImpl&) = default;
	MultiDetectorImpl(MultiDetectorImpl&&) = default;

	/*!
	 * @brief Perform logical disjunction with refine decisions along the chain
	 */
	template <typename TRegion>
	KOKKOS_INLINE_FUNCTION
	bool
	refine(const TRegion& region, BoolVec<TRegion>& result) const
	{
		BoolVec<TRegion> resHead, resTail;
		auto retHead = _detector(Head::refineTag, region, resHead);
		auto retTail = Tail::refine(region, resTail);
		bool ret = retHead || retTail;
		if (ret) {
			constexpr std::size_t N = BoolVec<TRegion>::size();
			for (std::size_t i = 0; i < N; ++i) {
				result[i] = resHead[i] || resTail[i];
			}
		}
		return ret;
	}

	/*!
	 * @brief Perform logical conjunction with select decisions along the chain
	 */
	template <typename TRegion>
	KOKKOS_INLINE_FUNCTION
	bool
	select(const TRegion& region) const
	{
		auto retHead = _detector(Head::selectTag, region);
		auto retTail = Tail::select(region);
		return retHead && retTail;
	}

private:
	//! My detector
	Head _detector;
};
} // namespace detail

/*!
 * @brief A single detector which combines the results of multiple detectors
 */
template <typename... TDetectors>
class MultiDetector : public Detector<MultiDetector<TDetectors...>>
{
public:
	//! Alias for parent class type
	using Superclass = Detector<MultiDetector<TDetectors...>>;

	template <typename TRegion>
	using BoolVec = typename Superclass::template BoolVec<TRegion>;

	MultiDetector(const MultiDetector&) = default;
	MultiDetector(MultiDetector&&) = default;

	/*!
	 * @brief construct with detectors to use
	 * @warning Number and order of provided detectors must match type list
	 * given as class template arguments
	 */
	template <typename... Ts>
	MultiDetector(Ts&&... detectors) : _impl(std::forward<Ts>(detectors)...)
	{
		static_assert(sizeof...(Ts) == sizeof...(TDetectors),
			"The number of detectors given to constructor must match the "
			"number of detector types");
	}

	/*!
	 * @brief Perform a logical disjunction with refine decisions from all
	 * detectors
	 */
	template <typename TRegion>
	KOKKOS_INLINE_FUNCTION
	bool
	refine(const TRegion& region, BoolVec<TRegion>& result) const
	{
		return _impl.refine(region, result);
	}

	/*!
	 * @brief Perform a logical conjunction with select decisions from all
	 * detectors
	 */
	template <typename TRegion>
	KOKKOS_INLINE_FUNCTION
	bool
	select(const TRegion& region) const
	{
		return _impl.select(region);
	}

private:
	//! Implementation
	detail::MultiDetectorImpl<TDetectors...> _impl;
};

/*!
 * @relates MultiDetector
 * @brief Construct a MultiDetector using template argument deduction (similar
 * to std::make_pair)
 */
template <typename... TDetectors>
MultiDetector<TDetectors...>
makeMultiDetector(TDetectors&&... detectors)
{
	return MultiDetector<TDetectors...>(std::forward<TDetectors>(detectors)...);
}
} // namespace refine
} // namespace plsm
