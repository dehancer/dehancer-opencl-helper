
INCLUDE(${clHelper_DIR}/clHelper.cmake)
COMPILE_OPENCL(../shaders/exampleKernel.cl)

ADD_EXECUTABLE(example_hello
        helloKernel.c
        ${EMBEDDED_OPENCL_KERNELS}
        )

TARGET_LINK_LIBRARIES(example_hello
        clHelperLib
        )

