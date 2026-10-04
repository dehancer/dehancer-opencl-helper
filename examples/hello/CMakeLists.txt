compile_opencl(helloKernel.cl)

add_executable(example_hello
    helloKernel.cpp
    ${EMBEDDED_OPENCL_KERNELS}
)

target_link_libraries(example_hello
    PRIVATE dehancer_opencl_helper::dehancer_opencl_helper
)
