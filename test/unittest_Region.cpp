#include <catch.hpp>

#include <plsm/Region.h>
#include <plsm/TestingCommon.h>
using namespace plsm;

TEMPLATE_LIST_TEST_CASE("Region Construction", "[Region][template]",
    test::IntTypes)
{
    Region<TestType, 3> r0;
    REQUIRE(r0.empty());

    using Ival = Interval<TestType>;
    Region<TestType, 3> r({Ival{0, 1}, Ival{0, 1}, Ival{0, 1}});
    static_assert(r.dimension() == 3, "");
    REQUIRE(r.size() == 3);
    REQUIRE(!r.empty());
    REQUIRE(r[0] == Ival{0, 1});
    REQUIRE(r[1] == Ival{0, 1});
    REQUIRE(r[2] == Ival{0, 1});
    REQUIRE(r.isSimplex());
    REQUIRE(r.volume() == 1);
    REQUIRE(r.getOrigin() == SpaceVector<TestType, 3>{0, 0, 0});

    Region<TestType, 3> r2({Ival{0, 2}, Ival{2, 4}, Ival{4, 6}});
    REQUIRE(r2[0] == Ival{0, 2});
    REQUIRE(r2[1] == Ival{2, 4});
    REQUIRE(r2[2] == Ival{4, 6});
    REQUIRE(!r2.isSimplex());
    REQUIRE(r2.volume() == 8);
    REQUIRE(r2.getOrigin() == SpaceVector<TestType, 3>{0, 2, 4});

    Region<TestType, 3> r3(SpaceVector<TestType, 3>{});
    REQUIRE(r3 == r);
    REQUIRE(r3[0] == Ival{0, 1});
    REQUIRE(r3[1] == Ival{0, 1});
    REQUIRE(r3[2] == Ival{0, 1});
    REQUIRE(r3.getOrigin() == SpaceVector<TestType, 3>{0, 0, 0});

    Region<TestType, 3> r4(SpaceVector<TestType, 3>{3, 4, 5});
    REQUIRE(r4[0] == Ival{3, 4});
    REQUIRE(r4[1] == Ival{4, 5});
    REQUIRE(r4[2] == Ival{5, 6});
    REQUIRE(r4.getOrigin() == SpaceVector<TestType, 3>{3, 4, 5});
}

TEMPLATE_LIST_TEST_CASE("Region Contains point", "[Region][template]",
    test::IntTypes)
{
    SpaceVector<TestType, 3> p{};

    Region<TestType, 3> r0;
    REQUIRE(!r0.contains(p));

    using Ival = Interval<TestType>;
    Region<TestType, 3> r({Ival{0, 1}, Ival{0, 1}, Ival{0, 1}});
    REQUIRE(r.size() == 3);
    REQUIRE(r.contains(p));

    Region<TestType, 3> r2({Ival{0, 2}, Ival{2, 4}, Ival{4, 6}});
    REQUIRE(!r2.contains(p));
    p = {1, 3, 5};
    REQUIRE(r2.contains(p));
    p[0] = 2;
    REQUIRE(!r2.contains(p));
}

TEMPLATE_LIST_TEST_CASE("Region Intersection", "[Region][template]",
    test::IntTypes)
{
    using Ival = Interval<TestType>;
    constexpr auto wc = wildcard<TestType>;

    Region<TestType, 3> r1({Ival{0, 1}, Ival{0, 1}, Ival{0, 1}});
    Region<TestType, 3> r2({Ival{0, 2}, Ival{2, 4}, Ival{4, 6}});
    REQUIRE(!r2.intersects(r1));
    REQUIRE(!r1.intersects(r2));
    Region<TestType, 3> r3({Ival{1, 3}, Ival{1, 3}, Ival{3, 5}});
    REQUIRE(r2.intersects(r3));
    REQUIRE(r3.intersects(r2));

    SpaceVector<TestType, 3> flv{2, 2, wc};
    REQUIRE(r3.intersects(flv));
    REQUIRE(!r1.intersects(flv));
    REQUIRE(!r2.intersects(flv));

    SpaceVector<TestType, 3> flu;
    flu = {0, 0, wc};
    flv = {5, 5, wc};
    REQUIRE(r1.intersects(flu));
    REQUIRE(r1.intersects(flu, flv));
    REQUIRE(!r2.intersects(flu, flv));
    REQUIRE(r3.intersects(flu, flv));

    flu = {2, 0, wc};
    flv = {0, 2, wc};
    REQUIRE(r3.intersects(flu, flv));
    REQUIRE(r3.intersects(flv, flu));
    REQUIRE(r2.intersects(flu, flv));
    REQUIRE(r2.intersects(flv, flu));
    REQUIRE(!r1.intersects(flu, flv));
    REQUIRE(!r1.intersects(flv, flu));

    flu = {wc, 0, 0};
    REQUIRE(r1.intersects(flu));
    REQUIRE(!r2.intersects(flu));
    REQUIRE(!r3.intersects(flu));
    flv = {wc, 3, 5};
    REQUIRE(r3.intersects(flu, flv));

    REQUIRE(r2.intersects(r3));
    REQUIRE(r3.intersects(r2));
    Region<TestType, 3> r4({Ival{1, 3}, Ival{1, 3}, Ival{1,3}});
    REQUIRE(!r1.intersects(r4));
    REQUIRE(!r4.intersects(r1));
    Region<TestType, 3> r5({Ival{0, 2}, Ival{0, 2}, Ival{0, 2}});
    REQUIRE(r5.intersects(r4));
    REQUIRE(r4.intersects(r5));
    Region<TestType, 3> r6({Ival{10}, Ival{10}, Ival{10}});
    REQUIRE(r6.intersects(r4));
    REQUIRE(r4.intersects(r6));
}

TEMPLATE_LIST_TEST_CASE("Region Geometry", "[Region][template]", test::IntTypes)
{
    Region<TestType, 3> r0;
    REQUIRE(r0.empty());

    using Ival = Interval<TestType>;
    using Vecd = SpaceVector<double, 3>;

    Region<TestType, 3> r({Ival{0, 1}, Ival{0, 1}, Ival{0, 1}});
    REQUIRE(r[0].midpoint() == 0.0);
    REQUIRE(r[1].midpoint() == 0.0);
    REQUIRE(r[2].midpoint() == 0.0);
    REQUIRE(r.dispersion() == Vecd{1.0, 1.0, 1.0});

    Region<TestType, 3> r2({Ival{0, 2}, Ival{2, 4}, Ival{4, 6}});
    REQUIRE(r2[0].midpoint() == 0.5);
    REQUIRE(r2[1].midpoint() == 2.5);
    REQUIRE(r2[2].midpoint() == 4.5);

    Region<TestType, 3> r4(SpaceVector<TestType, 3>{3, 4, 5});
    REQUIRE(r4[0].midpoint() == 3.0);
    REQUIRE(r4[1].midpoint() == 4.0);
    REQUIRE(r4[2].midpoint() == 5.0);
}
