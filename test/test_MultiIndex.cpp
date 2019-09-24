#include <catch.hpp>

//plsm
#include <plsm/MultiIndex.h>
#include <plsm/TestingCommon.h>
using namespace plsm;

TEST_CASE("MultiIndex 2D")
{
    MultiIndex<2> i2{};
    MultiIndex<2> j2{2, 3};
    auto k2 = MultiIndex<2>::filled(1);
    REQUIRE(i2.size() == 2);
    REQUIRE(j2.size() == 2);
    REQUIRE(k2.size() == 2);
    REQUIRE(i2[0] == 0);
    REQUIRE(i2[1] == 0);
    REQUIRE(j2[0] == 2);
    REQUIRE(j2[1] == 3);
    REQUIRE(k2[0] == 1);
    REQUIRE(k2[1] == 1);

    i2 = {1, 1};
    REQUIRE(i2[0] == 1);
    REQUIRE(i2[1] == 1);
    REQUIRE(i2 == k2);

    auto ij = j2 - i2;
    REQUIRE(ij[0] == 1);
    REQUIRE(ij[1] == 2);
}

TEST_CASE("MultiIndex 3D")
{
    MultiIndex<3> i3{};
    MultiIndex<3> j3{2, 3, 4};
    auto k3 = MultiIndex<3>::filled(1);
    REQUIRE(i3.size() == 3);
    REQUIRE(j3.size() == 3);
    REQUIRE(k3.size() == 3);
    REQUIRE(i3[0] == 0);
    REQUIRE(i3[1] == 0);
    REQUIRE(i3[2] == 0);
    REQUIRE(j3[0] == 2);
    REQUIRE(j3[1] == 3);
    REQUIRE(j3[2] == 4);
    REQUIRE(k3[0] == 1);
    REQUIRE(k3[1] == 1);
    REQUIRE(k3[2] == 1);

    i3 = {1, 1, 1};
    REQUIRE(i3[0] == 1);
    REQUIRE(i3[1] == 1);
    REQUIRE(i3[2] == 1);
    REQUIRE(i3 == k3);

    auto ij = j3 - i3;
    REQUIRE(ij[0] == 1);
    REQUIRE(ij[1] == 2);
    REQUIRE(ij[2] == 3);
}
