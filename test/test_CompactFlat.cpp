#define CATCH_CONFIG_MAIN
#include <catch.hpp>

//plsm
#include <plsm/CompactFlat.h>
#include <plsm/TestingCommon.h>
using namespace plsm;

TEMPLATE_LIST_TEST_CASE("2D", "[CompactFlat][template]", test::ScalarTypes)
{
    SpaceVector<TestType, 2> o{};
    SpaceVector<TestType, 2> x{8, wildcard<TestType>};
    SpaceVector<TestType, 2> y{wildcard<TestType>, 5};

    CompactFlat<TestType, 2> origin{o};
    CompactFlat<TestType, 2> x8{x};
    CompactFlat<TestType, 2> y5{y};

    REQUIRE(origin.dimension() == 0);
    REQUIRE(origin.size() == 2);
    REQUIRE(x8.dimension() == 1);
    REQUIRE(x8.size() == 1);
    REQUIRE(y5.dimension() == 1);
    REQUIRE(y5.size() == 1);
    REQUIRE(x8.expandCoordinate(0) == 0);
    REQUIRE(y5.expandCoordinate(0) == 1);
    REQUIRE(origin.expand() == o);
    REQUIRE(x8.expand() == x);
    REQUIRE(y5.expand() == y);
    REQUIRE(x8[0] == 8);
    REQUIRE(y5[0] == 5);
}

TEMPLATE_LIST_TEST_CASE("3D", "[CompactFlat][template]", test::ScalarTypes)
{
    SpaceVector<TestType, 3> o{};

    CompactFlat<TestType, 3> origin{o};

    REQUIRE(origin.dimension() == 0);
    REQUIRE(origin.size() == 3);
    REQUIRE(origin.expandCoordinate(0) == 0);
    REQUIRE(origin.expandCoordinate(1) == 1);
    REQUIRE(origin.expandCoordinate(2) == 2);
    REQUIRE(origin.expand() == o);

    SpaceVector<TestType, 3> xy{8, 9, wildcard<TestType>};
    SpaceVector<TestType, 3> yz{wildcard<TestType>, 3, 4};
    SpaceVector<TestType, 3> xz{0, wildcard<TestType>, 1};

    CompactFlat<TestType, 3> zLine{xy};
    CompactFlat<TestType, 3> xLine{yz};
    CompactFlat<TestType, 3> yLine{xz};

    REQUIRE(zLine.dimension() == 1);
    REQUIRE(zLine.size() == 2);
    REQUIRE(xLine.dimension() == 1);
    REQUIRE(xLine.size() == 2);
    REQUIRE(yLine.dimension() == 1);
    REQUIRE(yLine.size() == 2);
    REQUIRE(zLine.expandCoordinate(0) == 0);
    REQUIRE(zLine.expandCoordinate(1) == 1);
    REQUIRE(xLine.expandCoordinate(0) == 1);
    REQUIRE(xLine.expandCoordinate(1) == 2);
    REQUIRE(yLine.expandCoordinate(0) == 0);
    REQUIRE(yLine.expandCoordinate(1) == 2);
    REQUIRE(zLine.expand() == xy);
    REQUIRE(xLine.expand() == yz);
    REQUIRE(yLine.expand() == xz);
    REQUIRE(zLine[0] == 8);
    REQUIRE(zLine[1] == 9);
    REQUIRE(xLine[0] == 3);
    REQUIRE(xLine[1] == 4);
    REQUIRE(yLine[0] == 0);
    REQUIRE(yLine[1] == 1);

    SpaceVector<TestType, 3> x{8, wildcard<TestType>, wildcard<TestType>};
    SpaceVector<TestType, 3> y{wildcard<TestType>, 5, wildcard<TestType>};
    SpaceVector<TestType, 3> z{wildcard<TestType>, wildcard<TestType>, 3};

    CompactFlat<TestType, 3> yzPlane{x};
    CompactFlat<TestType, 3> xzPlane{y};
    CompactFlat<TestType, 3> xyPlane{z};

    REQUIRE(yzPlane.dimension() == 2);
    REQUIRE(yzPlane.size() == 1);
    REQUIRE(xzPlane.dimension() == 2);
    REQUIRE(xzPlane.size() == 1);
    REQUIRE(xyPlane.dimension() == 2);
    REQUIRE(xyPlane.size() == 1);
    REQUIRE(yzPlane.expandCoordinate(0) == 0);
    REQUIRE(xzPlane.expandCoordinate(0) == 1);
    REQUIRE(xyPlane.expandCoordinate(0) == 2);
    REQUIRE(yzPlane.expand() == x);
    REQUIRE(xzPlane.expand() == y);
    REQUIRE(xyPlane.expand() == z);
    REQUIRE(yzPlane[0] == 8);
    REQUIRE(xzPlane[0] == 5);
    REQUIRE(xyPlane[0] == 3);
}
