cmake_minimum_required(VERSION 3.25)

include("${CMAKE_CURRENT_LIST_DIR}/git.cmake")

git_get_semver(
    OUTPUT RSP_CORE_LIB_SEMVER
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

configure_file("${VERSION_H_IN}" "${VERSION_H_OUT}" @ONLY)
