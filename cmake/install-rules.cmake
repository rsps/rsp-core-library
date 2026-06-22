include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(RSP_CORE_LIB_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    CACHE STRING "Install destination for ${PROJECT_NAME} CMake package files"
)

# Install the library target and record it in an export set.
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Public headers (source tree).
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.h.in" EXCLUDE
)

# Generated headers (config.h, version.h).
install(
    DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.h"
)

# Export set: <prefix>/lib/cmake/rsp-core-lib/rsp-core-libTargets.cmake
install(
    EXPORT ${PROJECT_NAME}Targets
    FILE "${PROJECT_NAME}Targets.cmake"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "${RSP_CORE_LIB_INSTALL_CMAKEDIR}"
)

# CMake helper modules shipped with this package
install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/parse-version-file.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindSDL2.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Findwpa_supplicant.cmake"
    DESTINATION "${RSP_CORE_LIB_INSTALL_CMAKEDIR}"
)

# Generate the package config file from the template
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${RSP_CORE_LIB_INSTALL_CMAKEDIR}"
)

# Generate the package version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "${RSP_CORE_LIB_INSTALL_CMAKEDIR}"
)

# Make the package usable directly from the build tree (without installing).
export(
    EXPORT ${PROJECT_NAME}Targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
    NAMESPACE "${PROJECT_NAME}::"
)
