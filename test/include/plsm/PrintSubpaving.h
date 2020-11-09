#pragma once

#include <iostream>

#include <plsm/Subpaving.h>

namespace plsm
{
namespace test
{
template <typename TScalar, std::size_t Dim, typename TItemData>
void
printSubpaving(Subpaving<TScalar, Dim, TItemData>& subpaving, std::ostream& os)
{
	subpaving.syncAll(onHost);
	auto tiles = subpaving.getTiles();
	auto zones = subpaving.getZones();
	os << "Zones: " << zones.size() << '\n';
	for (std::size_t i = 0; i < zones.size(); ++i) {
		const auto& zone = zones(i);
		os << i << ": reg " << zone.getRegion() << "; subs "
		   << zone.getSubZoneIndices();
		if (zone.hasParent()) {
			os << "; parent " << zone.getParentIndex();
		}
		if (zone.hasTile()) {
			os << "; tile " << zone.getTileIndex();
		}
		os << '\n';
	}
}

template <typename TScalar, std::size_t Dim, typename TItemData>
void
printSubpaving(Subpaving<TScalar, Dim, TItemData>& subpaving)
{
	printSubpaving(subpaving, std::cout);
}
} // namespace test
} // namespace plsm
