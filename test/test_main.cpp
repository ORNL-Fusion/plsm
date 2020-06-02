#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#include <Kokkos_Core.hpp>

int
main(int argc, char* argv[])
{
    Kokkos::ScopeGuard kokkosContext;
    return Catch::Session().run(argc, argv);
}
