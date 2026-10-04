compile_opencl(exampleKernel.cl)

opencl_include_directories(
    ./
    ../shaders
)

add_executable(example_aoBench
    aoBench.cpp
    Function.cpp
    Image.cpp
    ${EMBEDDED_OPENCL_KERNELS}
)

target_link_libraries(example_aoBench
    PRIVATE dehancer_opencl_helper::dehancer_opencl_helper
)
