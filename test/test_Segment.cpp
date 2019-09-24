#include <catch.hpp>

//plsm
#include <plsm/Segment.h>
#include <plsm/SpaceVector.h>
#include <plsm/TestingCommon.h>
using namespace plsm;

TEMPLATE_LIST_TEST_CASE("Segment Basic", "[Segment][template]", test::IntTypes)
{
    SpaceVector<TestType, 3> a{};
    SpaceVector<TestType, 3> b{1, 1, 1};
    using SegmentType = Segment<SpaceVector<TestType, 3>>;
    using VectorType = typename SegmentType::VectorType;
    // auto ab = static_cast<VectorType>(b) - static_cast<VectorType>(a);
    SegmentType seg{a, b};
    REQUIRE(seg.origin() == a);
    REQUIRE(seg.vector() == VectorType{1, 1, 1});

    a = {4, 4, 4};
    b = {12, 12, 4};
    SegmentType seg1{a, b};
    REQUIRE(seg1.origin() == a);
    REQUIRE(seg1.vector() == VectorType{8, 8, 0});

    // REQUIRE(
}
