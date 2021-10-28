#pragma once

#include <Kokkos_Macros.hpp>

#include <plsm/Utility.h>

namespace plsm
{
namespace refine
{
/*!
 * Tag for straightforward refinement decision
 */
struct Refine
{
};

/*!
 * Tag for determining if a region intersects with the detector geometry; can be
 * used for refinement or selection
 */
struct Intersect
{
};

/*!
 * Tag for determining if a region overlaps with the detector geometry; can be
 * used for refinement or selection
 */
struct Overlap
{
};

/*!
 * Tag for straightforward selection deciesion
 */
struct Select
{
};

/*!
 * Tag for overload that always returns `true`
 */
struct SelectAll
{
};

/*!
 * @relates Detector
 * Pair of tag types for refinement and selection; for use as template argument
 * to Detector
 *
 * @tparam TRefine Refine tag
 * @tparam TSelect Select tag
 */
template <typename TRefine, typename TSelect>
struct TagPair
{
};

/*!
 * @relates Detector
 * Specialization of TagPair using Intersect for refinement and Overlap for
 * selection
 */
using IntersectAndOverlap =
	TagPair<::plsm::refine::Intersect, ::plsm::refine::Overlap>;

namespace detail
{
/*!
 * Helper struct for defining the refinement and selection tags for the
 * Detector
 */
template <typename TRefineTag = void>
struct RefineTraits
{
};

/*!
 * Default instantiation uses Refine and Select
 */
template <>
struct RefineTraits<void>
{
	//! Refine
	using RefineTag = ::plsm::refine::Refine;
	//! Select
	using SelectTag = ::plsm::refine::Select;
};

/*!
 * Specify both tags using a TagPair
 */
template <typename TRefine, typename TSelect>
struct RefineTraits<TagPair<TRefine, TSelect>>
{
	//! Use the first argument to TagPair for refinement
	using RefineTag = TRefine;
	//! Use the second argument to TagPair for selection
	using SelectTag = TSelect;
};

/*!
 * Specify Intersect for refinement
 */
template <>
struct RefineTraits<::plsm::refine::Intersect>
{
	//! Use Intersect for refinement
	using RefineTag = ::plsm::refine::Intersect;
	//! Select
	using SelectTag = ::plsm::refine::Select;
};

/*!
 * Specify Overlap for refinement
 */
template <>
struct RefineTraits<::plsm::refine::Overlap>
{
	//! Use Overlap for refinement
	using RefineTag = ::plsm::refine::Overlap;
	//! Select
	using SelectTag = ::plsm::refine::Select;
};
} // namespace detail

/*!
 * @brief Detector provides a means of customizing the decision made in
 * detail::Refiner of whether to refine or select a given Region.
 *
 * The naming assumes that a given child class detector implementation may use
 * different forms of intuition for the decision to refine or select a given
 * Region. One may use some geometric entity, which would Overlap or Intersect
 * the domain of interest. On the other hand, some non-geometric logic
 * may be used to decide directly whether to Refine and whether to Select. The
 * interface therefore provides multiple names that may be implemented. The user
 * need implement only those functions that make intuitive sense for their
 * context (see BallDetector, RegionDetector, PolylineDetector).
 *
 * The tag mechanism allows the user to specify (for a given child detector
 * type) which method to use for refinement and selection. For example, if the
 * detector is using a circle geometry, it could implement overlap() and
 * intersect(). Then the user could specify either Overlap or Intersect as the
 * refinement tag to refine across the interior of the circle or just the
 * boundary.
 *
 * The RefineTag and SelectTag member types are used by the Refiner to call the
 * appropriate overload of the call operator.
 *
 * @tparam TDerived (CRTP) name of child class to be used for custom refinement
 * and selection decisions
 * @tparam TRefineTag tag specifier passed to RefineTraits; used to specify
 * refinement and selection tags
 *
 * @test unittest_Detectors.cpp
 * @test unittest_Subpaving.cpp
 * @test benchmark_Subpaving.cpp
 */
template <typename TDerived, typename TRefineTag = void>
class Detector
{
public:
	//! Alias for RefineTraits instance based on TRefineTag
	using Traits = ::plsm::refine::detail::RefineTraits<TRefineTag>;
	//! Tag specifying refinement method
	using RefineTag = typename Traits::RefineTag;
	//! Tag specifying selection method
	using SelectTag = typename Traits::SelectTag;

	/*!
	 * @brief Set of bool flags, one for each axis
	 */
	template <typename TRegion>
	using BoolVec = Kokkos::Array<bool, TRegion::dimension()>;

	//! Tag instance for refinement
	static constexpr RefineTag refineTag{};
	//! Tag instance for selection
	static constexpr SelectTag selectTag{};

	//! Special value to specify no level limit on refinement
	static constexpr std::size_t fullDepth = wildcard<std::size_t>;

	/*!
	 * @brief Default constructor uses fullDepth
	 */
	Detector() = default;

	/*!
	 * @brief Construct with specified refinement depth
	 */
	explicit Detector(std::size_t refineDepth) : _depth{refineDepth}
	{
	}

	/*!
	 * @brief Get refinement depth
	 */
	KOKKOS_INLINE_FUNCTION
	std::size_t
	depth() const noexcept
	{
		return _depth;
	}

	/*!
	 * Decision operator
	 *
	 * Use tag dispatch overloads to provide a common interface for all custom
	 * decision function implementations.
	 *
	 * This operator makes use of the CRTP to directly call the derived class
	 * implementation of the tagged decision function
	 *
	 * The detail::Refiner will call this operator with either the refineTag
	 * or selectTag member in order to choose the desired implementation
	 *
	 * @note The variadic parameter pack allows these decision functions to be
	 * implemented either to return a single boolean result or to fill in a
	 * BoolVec out parameter. The BoolVec version specifies a separate decision
	 * for each dimension of a Region. This allows the user to tell the
	 * detail::Refiner to refine on some axes but not on others (as opposed to
	 * simply refining in all axes or not at all).
	 */
	template <typename... TArgs>
	bool
	operator()(TArgs&&...) const;

	/*!
	 * @brief With Refine tag, forward to refine()
	 * @copydetails operator()()
	 */
	template <typename... TArgs>
	KOKKOS_INLINE_FUNCTION
	bool
	operator()(::plsm::refine::Refine, TArgs&&... args) const
	{
		return asDerived()->refine(std::forward<TArgs>(args)...);
	}

	/*!
	 * @brief With Intersect tag, forward to intersect()
	 * @copydetails operator()()
	 */
	template <typename... TArgs>
	KOKKOS_INLINE_FUNCTION
	bool
	operator()(::plsm::refine::Intersect, TArgs&&... args) const
	{
		return asDerived()->intersect(std::forward<TArgs>(args)...);
	}

	/*!
	 * @brief With Overlap tag, forward to overlap()
	 * @copydoc operator()()
	 */
	template <typename... TArgs>
	KOKKOS_INLINE_FUNCTION
	bool
	operator()(::plsm::refine::Overlap, TArgs&&... args) const
	{
		return asDerived()->overlap(std::forward<TArgs>(args)...);
	}

	/*!
	 * @brief With Select tag, forward to select()
	 * @copydoc operator()()
	 */
	template <typename... TArgs>
	KOKKOS_INLINE_FUNCTION
	bool
	operator()(::plsm::refine::Select, TArgs&&... args) const
	{
		return asDerived()->select(std::forward<TArgs>(args)...);
	}

	/*!
	 * @brief With SelectAll tag, return `true`
	 * @copydoc operator()()
	 */
	template <typename... TArgs>
	KOKKOS_INLINE_FUNCTION
	bool
	operator()(::plsm::refine::SelectAll, TArgs&&...) const
	{
		return true;
	}

	/*!
	 * @brief Set each element of the given BoolVec with the single given value
	 * @param[in] value Boolean result to apply
	 * @param[out] result BoolVec to fill with value
	 * @returns value
	 */
	template <typename TBoolVec>
	KOKKOS_INLINE_FUNCTION
	bool
	applyResult(bool value, TBoolVec& result) const
	{
		for (std::size_t i = 0; i < result.size(); ++i) {
			result[i] = value;
		}
		return value;
	}

	/*!
	 * If the derived class does not implement a BoolVec version of this
	 * function, this implementation is called, which calls applyResult() with
	 * the returned value from the derived class single result version
	 */
	template <typename TRegion, typename TBoolVec = BoolVec<TRegion>>
	KOKKOS_INLINE_FUNCTION
	bool
	refine(TRegion&& region, TBoolVec& result) const
	{
		return applyResult(asDerived()->refine(region), result);
	}

	/*!
	 * @copydoc refine()
	 */
	template <typename TRegion, typename TBoolVec = BoolVec<TRegion>>
	KOKKOS_INLINE_FUNCTION
	bool
	intersect(TRegion&& region, TBoolVec& result) const
	{
		return applyResult(asDerived()->intersect(region), result);
	}

	/*!
	 * @copydoc refine()
	 */
	template <typename TRegion, typename TBoolVec = BoolVec<TRegion>>
	KOKKOS_INLINE_FUNCTION
	bool
	overlap(TRegion&& region, TBoolVec& result) const
	{
		return applyResult(asDerived()->overlap(region), result);
	}

protected:
	/*!
	 * @brief static_cast to TDerived
	 */
	KOKKOS_INLINE_FUNCTION
	const TDerived*
	asDerived() const noexcept
	{
		return static_cast<const TDerived*>(this);
	}

protected:
	//! Limit on refinement depth
	std::size_t _depth{fullDepth};
};
} // namespace refine
} // namespace plsm
