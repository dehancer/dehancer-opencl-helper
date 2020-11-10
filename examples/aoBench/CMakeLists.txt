
INCLUDE(${clHelper_DIR}/clHelper.cmake)

COMPILE_OPENCL(
        ../shaders/exampleKernel.cl
)

OPENCL_INCLUDE_DIRECTORIES(
        ./
        ../shaders
)

ADD_EXECUTABLE(example_aoBench
        aoBench.cpp
        Function.cpp
        Image.cpp
        ${EMBEDDED_OPENCL_KERNELS}
        )

TARGET_LINK_LIBRARIES(example_aoBench
        clHelperLib
        )

