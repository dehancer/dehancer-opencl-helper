
INCLUDE(${clHelper_DIR}/clHelper.cmake)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/clHelper)

COMPILE_OPENCL(helloKernel.cl)

ADD_EXECUTABLE(example_hello
        helloKernel.cpp
        ${EMBEDDED_OPENCL_KERNELS}
        )

TARGET_LINK_LIBRARIES(example_hello
        clHelperLib
        )

