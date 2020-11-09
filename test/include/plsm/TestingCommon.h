#pragma once

#include <cstdint>
#include <tuple>

namespace plsm
{
namespace test
{
using IntTypes = std::tuple<int, long, long long, unsigned, unsigned long,
	unsigned long long, std::size_t, std::ptrdiff_t, std::int32_t, std::int64_t,
	std::uint32_t, std::uint64_t>;

using UIntTypes = std::tuple<unsigned, unsigned long, unsigned long long,
	std::size_t, std::uint32_t, std::uint64_t>;

using OtherScalars = std::tuple<double, float>;

using ScalarTypes = decltype(std::tuple_cat(IntTypes{}, OtherScalars{}));

enum class Axis
{
	x,
	y,
	z
};
} // namespace test
} // namespace plsm
