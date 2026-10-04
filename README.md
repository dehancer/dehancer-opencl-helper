# dehancer-opencl-helper

## Build and install

Requires OpenCL 3.0 development headers and loader. Windows also requires
the `dlfcn-win32` CMake package (`dlfcn-win32::dl`).

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)
cmake --install build --parallel $(nproc)
```

Make sure to set proper `CMAKE_PREFIX_PATH` and `CMAKE_INSTALL_PREFIX` to discover dependencies and install.

`CMAKE_POSITION_INDEPENDENT_CODE` is set to `ON`.

The library remains static, named `clHelperLib`.
Existing `OpenCL::OpenCL` and `dlfcn-win32::dl` targets are reused.

Examples are opt-in through `OPENCL_BUILD_EXAMPLES`.

## Embedded kernels

All three modes expose `COMPILE_OPENCL`, `OPENCL_INCLUDE_DIRECTORIES`, and
`OPENCL_ADD_DEFINITION`. Kernel embedding requires `clang` and `xxd`. These
tools are discovered only when `COMPILE_OPENCL` is used. Enable C in the
consuming project for generated C:

```cmake
COMPILE_OPENCL(kernel.cl)
target_sources(my_executable PRIVATE ${EMBEDDED_OPENCL_KERNELS})
```

Kernel symbols must remain visible to `dlsym`. The target propagates executable
export flags on Linux, FreeBSD, and macOS. Windows consumers must export their
embedded kernel symbols.

## Usage in CMake

```cmake
find_package(dehancer_opencl_helper CONFIG REQUIRED)
target_link_libraries(app PRIVATE dehancer_opencl_helper::dehancer_opencl_helper)
```

The target propagates headers, `CL_TARGET_OPENCL_VERSION=300`, OpenCL,
and required loader linkage.

## Original README

See [Original-README.md](Original-README.md).
