# Build and install

Requires CMake 4.3+, a C++17 compiler, and OpenCL 1.2 development headers and loader.
Windows also requires the `dlfcn-win32` CMake package (`dlfcn-win32::dl`).
macOS uses the SDK's OpenCL framework.

```sh
cmake -S . -B build \
  -G "Ninja Multi-Config" \
  -DOPENCL_HELPER_TEST=OFF \
  -DCMAKE_PREFIX_PATH=$HOME/local-dehancer \

cmake --build build --config Release --parallel $(nproc)
cmake --install build --config Release --prefix $HOME/local-dehancer
```

The library remains static, named `clHelperLib`. No dependencies are downloaded.
Existing `OpenCL::OpenCL` and `dlfcn-win32::dl` targets are reused. Examples are
opt-in through `OPENCL_HELPER_TEST`; parent `BUILD_TESTING` does not enable them.

`CMAKE_INSTALL_LIBDIR` and `CMAKE_INSTALL_INCLUDEDIR` select subdirectories.
Relative directories support relocation; absolute overrides remain fixed.

# CMake consumption

Installed package:

```cmake
find_package(dehancer_opencl_helper CONFIG REQUIRED)
target_link_libraries(my_library PRIVATE
    dehancer_opencl_helper::dehancer_opencl_helper
)
```

Set `CMAKE_PREFIX_PATH` to the installation prefix.

Source checkout:

```cmake
add_subdirectory(path/to/dehancer-opencl-helper)
target_link_libraries(my_library PRIVATE
    dehancer_opencl_helper::dehancer_opencl_helper
)
```

FetchContent with a local checkout:

```cmake
include(FetchContent)
FetchContent_Declare(dehancer_opencl_helper
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/dehancer-opencl-helper"
)
FetchContent_MakeAvailable(dehancer_opencl_helper)
target_link_libraries(my_library PRIVATE
    dehancer_opencl_helper::dehancer_opencl_helper
)
```

The target propagates headers, C++17, `CL_TARGET_OPENCL_VERSION=120`, OpenCL,
and required loader linkage. Use `PUBLIC` when your public headers expose these
headers. Include them as `<dehancer/opencl/device.h>`, for example.

# Embedded kernels

All three modes expose `COMPILE_OPENCL`, `OPENCL_INCLUDE_DIRECTORIES`, and
`OPENCL_ADD_DEFINITION`. Kernel embedding requires `clang` and `xxd`; optional
`ioc64` enables the legacy assembly/LLVM output. These tools are discovered only
when `COMPILE_OPENCL` is used. Enable C in the consuming project for generated C:

```cmake
COMPILE_OPENCL(kernel.cl)
target_sources(my_executable PRIVATE ${EMBEDDED_OPENCL_KERNELS})
```

Kernel symbols must remain visible to `dlsym`. The target propagates executable
export flags on Linux, FreeBSD, and macOS. Windows consumers must export their
embedded kernel symbols. The standalone `lib/cmake/clHelper.cmake` installation
path is retained.
