
INCLUDE(${clHelper_DIR}/clHelper.cmake)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/clHelper)

COMPILE_OPENCL(
        exampleKernel.cl
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
        ${CMAKE_DL_LIBS}
        )

