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

#### Catch2
Catch2 is used as the testing framework. It is included automatically since it
is header-only, and therefore nothing needs to be done by the user.

### CMake Instructions
```
cmake -DCMAKE_BUILD_TYPE=Release \
    -DKokkos_DIR=</path/to/kokkos/install> \
    /path/to/plsm
```

To build documentation:
```
make plsmdoc
```
