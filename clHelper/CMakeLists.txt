INCLUDE(clHelper.cmake)

FILE(GLOB PUBLIC_INCLUDE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        )

ADD_LIBRARY(clHelperLib STATIC
  embeddedProgram.cpp
  device.cpp
        platform.cpp
        )

TARGET_LINK_LIBRARIES(clHelperLib 
  ${OpenCL_LIBRARY}
  dl
  )

SET_PROPERTY(TARGET clHelperLib PROPERTY POSITION_INDEPENDENT_CODE ON)

install(TARGETS clHelperLib DESTINATION lib)
install(FILES  ${PUBLIC_INCLUDE_FILES} DESTINATION include/dehancer/opencl)
install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/clHelper.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
        )