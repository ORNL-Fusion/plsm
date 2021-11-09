#pragma once

#include <vector>

#include <Kokkos_Core.hpp>

#include <plsm/Region.h>
#include <plsm/refine/Detector.h>

namespace plsm
{
namespace refine
{
/*!
 * PolylineDetector is a Detector implementing refine() with respect to a
 * poly-hyperplane formed by a collection of CompactFlat objects
 *
 * @todo Implement in terms of interect and overlap (overlap would require some
 * sense of direction to be defined, that is, inside/outside)
 *
 * @test unittest_Detectors
 * @test benchmark_Subpaving.cpp
 */
template <typename TScalar, DimType Dim, typename TTag = void>
class PolylineDetector :
	public Detector<PolylineDetector<TScalar, Dim, TTag>, TTag>
{
public:
	//! Alias for parent class type
	using Superclass = Detector<PolylineDetector<TScalar, Dim, TTag>, TTag>;
	//! Underlying lattice scalar type
	using ScalarType = TScalar;
	//! Spatial point representation
	using PointType = SpaceVector<ScalarType, Dim>;
	//! Alias for CompactFlat
	using FlatType = CompactFlat<ScalarType, Dim>;
	//! Alias for Region
	using RegionType = Region<ScalarType, Dim>;

	using Superclass::Superclass;

	/*!
	 * @brief Construct with collection of points
	 */
	PolylineDetector(const std::vector<PointType>& polyline,
		std::size_t refineDepth = Superclass::fullDepth) :
		Superclass(refineDepth),
		_flats("Refinement Polyline Flats", polyline.size())
	{
		auto fMirror = Kokkos::create_mirror_view(_flats);
		std::copy(begin(polyline), end(polyline), fMirror.data());
		Kokkos::deep_copy(_flats, fMirror);
	}

	using Superclass::intersect;
	using Superclass::refine;

	/*!
	 * @brief Test for intersection with polyline
	 */
	KOKKOS_INLINE_FUNCTION
	bool
	intersect(const RegionType& region) const
	{
		return region.intersects(_flats);
	}

	/*!
	 * @brief Test for intersection with polyline
	 */
	KOKKOS_INLINE_FUNCTION
	bool
	refine(const RegionType& region) const
	{
		return intersect(region);
	}

	/*!
	 * @brief Apply SelectAll
	 */
	KOKKOS_INLINE_FUNCTION
	bool
	select(const RegionType& region) const
	{
		// return (*this)(SelectAll{}, region);

		// WARNING: this only works for 3D, z-aligned planes

		for (std::size_t i = 0; i < _flats.size() - 1; ++i) {
			const auto p0 = static_cast<CompactFlat<double, Dim>>(_flats[i]);
			const auto p1 =
				static_cast<CompactFlat<double, Dim>>(_flats[i + 1]);
			const auto x = static_cast<double>(region[0].begin());
			if (x < p0[0]) {
				continue;
			}
			if (x >= p1[0]) {
				continue;
			}
			auto m = (p1[1] - p0[1]) / (p1[0] - p0[0]);
			auto y = p0[1] + m * (x - p0[0]);
			if (static_cast<double>(region[1].end()) <= y) {
				return false;
			}
			return true;
		}
		return false;
	}

private:
	//! poly-hyperplane representation in terms of CompactFlat objects
	Kokkos::View<FlatType*> _flats;
};
} // namespace refine
} // namespace plsm
