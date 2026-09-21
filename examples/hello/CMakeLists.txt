COMPILE_OPENCL(helloKernel.cl)

ADD_EXECUTABLE(example_hello
        helloKernel.cpp
        ${EMBEDDED_OPENCL_KERNELS}
        )

TARGET_LINK_LIBRARIES(example_hello
        dehancer_opencl_helper::dehancer_opencl_helper
        )
