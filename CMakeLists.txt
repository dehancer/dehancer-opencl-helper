cmake_minimum_required(VERSION 4.3)

project(dehancer_opencl_helper VERSION 0.6.0 LANGUAGES C CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(BUILD_EXAMPLES "Build OpenCL helper examples" OFF)

if(NOT TARGET OpenCL::OpenCL)
    find_package(OpenCL 1.2 REQUIRED) # FIXME or 3.0?
endif()

if(WIN32 AND NOT TARGET dlfcn-win32::dl)
    find_package(dlfcn-win32 CONFIG REQUIRED)
endif()

add_subdirectory(clHelper)
include(clHelper/clHelper.cmake)

set(dehancer_opencl_helper_cmake_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/dehancer_opencl_helper")
configure_package_config_file(
    cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/dehancer_opencl_helperConfig.cmake"
    INSTALL_DESTINATION "${dehancer_opencl_helper_cmake_install_dir}"
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/dehancer_opencl_helperConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

install(EXPORT dehancer_opencl_helperTargets
    FILE dehancer_opencl_helperTargets.cmake
    NAMESPACE dehancer_opencl_helper::
    DESTINATION "${dehancer_opencl_helper_cmake_install_dir}"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/dehancer_opencl_helperConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/dehancer_opencl_helperConfigVersion.cmake"
    clHelper/clHelper.cmake
    DESTINATION "${dehancer_opencl_helper_cmake_install_dir}"
)

# Preserve the standalone kernel helper's installation path.
install(FILES clHelper/clHelper.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
