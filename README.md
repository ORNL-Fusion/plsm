# plsm - Parallel Lattice Subpaving Map
plsm is a generic library for spatial subdivision within an N-dimensional
lattice.

## Build

### Dependencies

#### Kokkos
At this point, Kokkos must be built separately. It should be installed (with
`make install`) so that CMake settings can be gathered from plsm.

Perhaps this should be done as part of the build project so that configuration
options would be controlled through CMake for plsm.

#### Boost.Timer
This is only required for testing. If you turn off the `BUILD_TESTING` CMake
variable, this is not required.

#### Catch2
Catch2 is used as the testing framework. It is included automatically since it
is header-only, and therefore nothing needs to be done by the user.

### CMake Instructions
For serial mode:
```
cmake -DCMAKE_BUILD_TYPE=Release \
    -DKOKKOS_DIR=</path/to/kokkos/install> \
    /path/to/plsm
```

For CUDA mode:
```
cmake -DCMAKE_CXX_COMPILER=<path/to/kokkos/install>/bin/nvcc_wrapper \
    -DCMAKE_BUILD_TYPE=Release \
    -DKOKKOS_DIR=</path/to/kokkos/install> \
    /path/to/plsm
```

To build documentation:
```
make plsmdoc
```
