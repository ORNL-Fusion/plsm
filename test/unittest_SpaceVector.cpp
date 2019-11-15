#include <catch.hpp>

#include <plsm/SpaceVector.h>
#include <plsm/TestingCommon.h>
using namespace plsm;

TEMPLATE_LIST_TEST_CASE("SpaceVector 2D", "[SpaceVector][template]",
    test::ScalarTypes)
{
    SpaceVector<TestType, 2> a{3, 3};
    SpaceVector<TestType, 2> b{5, 4};
    SpaceVector<TestType, 2> c{};
    SpaceVector<TestType, 2> d;
    d = b;

    SECTION("Constructed State")
    {
        REQUIRE(a.size() == 2);
        REQUIRE(a[0] == 3);
        REQUIRE(a[1] == 3);
        REQUIRE(b.size() == 2);
        REQUIRE(b[0] == 5);
        REQUIRE(b[1] == 4);
        REQUIRE(c[0] == 0);
        REQUIRE(c[1] == 0);
        REQUIRE(d[0] == 5);
        REQUIRE(d[1] == 4);
    }

    SECTION("Assignment")
    {
        a = {4, 4};
        b = {};
        c = {2, 1};
        REQUIRE(a[0] == 4);
        REQUIRE(a[1] == 4);
        REQUIRE(b[0] == 0);
        REQUIRE(b[1] == 0);
        REQUIRE(c[0] == 2);
        REQUIRE(c[1] == 1);
    }

    SECTION("Subtraction")
    {
        auto ab = b - a;
        REQUIRE(ab[0] == 2);
        REQUIRE(ab[1] == 1);
        auto ba = a - b;
        REQUIRE(ba[0] == static_cast<TestType>(-2));
        REQUIRE(ba[1] == static_cast<TestType>(-1));
        auto cd = d - c;
        REQUIRE(cd == d);
    }
}

TEMPLATE_LIST_TEST_CASE("SpaceVector 3D", "[SpaceVector][template]",
    test::ScalarTypes)
{
    SpaceVector<TestType, 3> a{3, 3, 3};
    SpaceVector<TestType, 3> b{5, 4, 3};
    SpaceVector<TestType, 3> c{};
    SpaceVector<TestType, 3> d;
    d = b;

    SECTION("Constructed State")
    {
        REQUIRE(a.size() == 3);
        REQUIRE(a[0] == 3);
        REQUIRE(a[1] == 3);
        REQUIRE(a[2] == 3);
        REQUIRE(b.size() == 3);
        REQUIRE(b[0] == 5);
        REQUIRE(b[1] == 4);
        REQUIRE(b[2] == 3);
        REQUIRE(c[0] == 0);
        REQUIRE(c[1] == 0);
        REQUIRE(c[2] == 0);
        REQUIRE(d[0] == 5);
        REQUIRE(d[1] == 4);
        REQUIRE(d[2] == 3);
    }

    SECTION("Assignment")
    {
        a = {4, 4, 4};
        b = {};
        c = {2, 1, 3};
        REQUIRE(a[0] == 4);
        REQUIRE(a[1] == 4);
        REQUIRE(a[2] == 4);
        REQUIRE(b[0] == 0);
        REQUIRE(b[1] == 0);
        REQUIRE(b[2] == 0);
        REQUIRE(c[0] == 2);
        REQUIRE(c[1] == 1);
        REQUIRE(c[2] == 3);
    }

    SECTION("Subtraction")
    {
        auto ab = b - a;
        REQUIRE(ab[0] == 2);
        REQUIRE(ab[1] == 1);
        REQUIRE(ab[2] == 0);
        auto ba = a - b;
        REQUIRE(ba[0] == static_cast<TestType>(-2));
        REQUIRE(ba[1] == static_cast<TestType>(-1));
        REQUIRE(ba[2] == 0);
        auto cd = d - c;
        REQUIRE(cd == d);
    }
}

TEMPLATE_LIST_TEST_CASE("SpaceVector Filled", "[SpaceVector][template]",
    test::ScalarTypes)
{
    auto v1 = SpaceVector<TestType, 1>::filled(8);
    REQUIRE(v1[0] == 8);
    auto v2 = SpaceVector<TestType, 2>::filled(8);
    REQUIRE(v2[0] == 8);
    REQUIRE(v2[1] == 8);
    auto v3 = SpaceVector<TestType, 3>::filled(8);
    REQUIRE(v3[0] == 8);
    REQUIRE(v3[1] == 8);
    REQUIRE(v3[2] == 8);
    auto v4 = SpaceVector<TestType, 4>::filled(8);
    REQUIRE(v4[0] == 8);
    REQUIRE(v4[1] == 8);
    REQUIRE(v4[2] == 8);
    REQUIRE(v4[3] == 8);
    auto v16 = SpaceVector<TestType, 16>::filled(8);
    for (std::size_t i = 0; i < 16; ++i) {
        REQUIRE(v16[i] == 8);
    }
}

TEMPLATE_LIST_TEST_CASE("SpaceVector Axis", "[SpaceVector][template]",
    test::IntTypes)
{
    SECTION("2D")
    {
        SpaceVector<TestType, 2> a{0, 2};
        SpaceVector<TestType, 2> b{2, 0};
        SpaceVector<TestType, 2> c{0, 0};
        SpaceVector<TestType, 2> d{2, 2};
        REQUIRE(a.isOnAxis(1));
        REQUIRE(!a.isOnAxis(0));
        REQUIRE(b.isOnAxis(0));
        REQUIRE(!b.isOnAxis(1));
        REQUIRE(!c.isOnAxis(0));
        REQUIRE(!c.isOnAxis(1));
        REQUIRE(!d.isOnAxis(0));
        REQUIRE(!d.isOnAxis(1));
    }

    SECTION("3D")
    {
        SpaceVector<TestType, 3> a{0, 2, 0};
        SpaceVector<TestType, 3> b{2, 0, 0};
        SpaceVector<TestType, 3> c{0, 0, 2};
        SpaceVector<TestType, 3> d{0, 0, 0};
        SpaceVector<TestType, 3> e{2, 2, 2};
        SpaceVector<TestType, 3> f{2, 0, 2};
        REQUIRE(a.isOnAxis(1));
        REQUIRE(!a.isOnAxis(0));
        REQUIRE(!a.isOnAxis(2));
        REQUIRE(b.isOnAxis(0));
        REQUIRE(!b.isOnAxis(1));
        REQUIRE(!b.isOnAxis(2));
        REQUIRE(!c.isOnAxis(0));
        REQUIRE(!c.isOnAxis(1));
        REQUIRE(c.isOnAxis(2));
        REQUIRE(!d.isOnAxis(0));
        REQUIRE(!d.isOnAxis(1));
        REQUIRE(!d.isOnAxis(2));
        REQUIRE(!e.isOnAxis(0));
        REQUIRE(!e.isOnAxis(1));
        REQUIRE(!e.isOnAxis(2));
        REQUIRE(!f.isOnAxis(0));
        REQUIRE(!f.isOnAxis(1));
        REQUIRE(!f.isOnAxis(2));
    }
}
