#pragma once

#include <plsm/Region.h>
#include <plsm/refine/Detector.h>

namespace plsm
{
namespace refine
{
/*!
 * RegionDetector is a Detector implementing intersect() and overlap() with
 * respect to a Region
 *
 * @test unittest_Detectors
 * @test unittest_Subpaving.cpp
 * @test benchmark_Subpaving.cpp
 */
template <typename TScalar, DimType Dim, typename TTag = void>
class RegionDetector : public Detector<RegionDetector<TScalar, Dim, TTag>, TTag>
{
public:
	//! Alias for parent class type
	using Superclass = Detector<RegionDetector<TScalar, Dim, TTag>, TTag>;
	//! Underlying lattice scalar type
	using ScalarType = TScalar;
	//! Alias for Region
	using RegionType = Region<ScalarType, Dim>;

	using Superclass::Superclass;

	/*!
	 * @brief Construct with Region
	 * @param region Reference Region for intersection testing
	 * @param refineDepth Level limit on refinement (defaults to
	 * Detector::fullDepth)
	 */
	RegionDetector(const RegionType& region,
		std::size_t refineDepth = Superclass::fullDepth) :
		Superclass(refineDepth), _region{region}
	{
	}

	/*!
	 * @brief Test for intersection of given Region with reference Region
	 * boundary
	 */
	KOKKOS_INLINE_FUNCTION
	bool
	intersect(const RegionType& region) const
	{
		bool ret = false;
		for (DimType i = 0; i < region.dimension(); ++i) {
			auto ival0 = region[i];
			auto ival1 = _region[i];
			if (!ival0.intersects(ival1)) {
				ret = false;
				break;
			}
			if (intersectEndPoints(ival0, ival1)) {
				ret = true;
			}
		}
		return ret;
	}

	using Superclass::overlap;

	/*!
	 * @brief Test for overlap between given Region and reference Region
	 */
	KOKKOS_INLINE_FUNCTION
	bool
	overlap(const RegionType& region) const
	{
		return _region.intersects(region);
	}

private:
	//! Alias for Region Interval
	using IntervalType = typename RegionType::IntervalType;

	/*!
	 * @brief Test for intersection of two intervals at one endpoint
	 *
	 * @return true if the intervals overlap but neither fully contains the
	 * other, false otherwise
	 */
	KOKKOS_INLINE_FUNCTION
	bool
	intersectEndPoints(
		const IntervalType& ival0, const IntervalType& ival1) const
	{
		if (ival0.empty() || ival1.empty()) {
			return false;
		}
		auto last0 = ival0.end() - 1;
		auto last1 = ival1.end() - 1;
		return (ival0.begin() < ival1.begin() && last0 > ival1.begin()) ||
			(ival0.begin() < last1 && ival0.end() > last1);
	}

private:
	//! Reference region
	RegionType _region;
};
} // namespace refine
} // namespace plsm
