
INCLUDE(${clHelper_DIR}/clHelper.cmake)
COMPILE_OPENCL(aoBench.cl)
OPENCL_INCLUDE_DIRECTORIES(
        ./
)
ADD_EXECUTABLE(example_aoBench
  aoBench.cpp
  ${EMBEDDED_OPENCL_KERNELS}
  )
TARGET_LINK_LIBRARIES(example_aoBench
  clHelperLib
  )

