#pragma once

//Kokkos
#include <Kokkos_Core.hpp>

namespace plsm
{
namespace test
{
class KokkosContext
{
public:
    KokkosContext()
    {
        ::Kokkos::initialize();
    }

    ~KokkosContext()
    {
        ::Kokkos::finalize();
    }
};
}
}
