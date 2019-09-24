#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
//plsm
#include <plsm/KokkosContext.h>

int
main(int argc, char* argv[])
{
    plsm::test::KokkosContext context;
    return Catch::Session().run(argc, argv);
}
