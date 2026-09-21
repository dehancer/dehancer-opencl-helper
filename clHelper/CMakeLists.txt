add_library(clHelperLib STATIC
    embeddedProgram.cpp
    device.cpp
    platform.cpp
)
add_library(dehancer_opencl_helper::dehancer_opencl_helper ALIAS clHelperLib)

target_compile_features(clHelperLib PUBLIC cxx_std_17)
target_compile_definitions(clHelperLib PUBLIC CL_TARGET_OPENCL_VERSION=120)
target_link_libraries(clHelperLib PUBLIC OpenCL::OpenCL)
target_sources(clHelperLib PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES
        dehancer/opencl/common.h
        dehancer/opencl/device.h
        dehancer/opencl/embeddedProgram.h
        dehancer/opencl/platform.h
)
set_target_properties(clHelperLib PROPERTIES
    EXPORT_NAME dehancer_opencl_helper
    CXX_EXTENSIONS OFF
    CXX_VISIBILITY_PRESET default
    POSITION_INDEPENDENT_CODE ON
)

if(WIN32)
    target_link_libraries(clHelperLib PRIVATE dlfcn-win32::dl)
else()
    target_link_libraries(clHelperLib PRIVATE ${CMAKE_DL_LIBS})
    # Embedded kernels are resolved from the executable through dlsym().
    target_link_options(clHelperLib INTERFACE
        "$<$<PLATFORM_ID:Linux,FreeBSD>:LINKER:--export-dynamic>"
        "$<$<PLATFORM_ID:Darwin>:LINKER:-export_dynamic>"
    )
endif()

install(TARGETS clHelperLib
    EXPORT dehancer_opencl_helperTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
