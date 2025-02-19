# -------------------------------------------------------------------------------------------------------------- #
# cURL
# @see https://curl.se/
#
# @see https://cmake.org/cmake/help/latest/module/FindCURL.html
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing cURL")

find_package(CURL 7.33 REQUIRED)

# Debug
message(STATUS "Using cURL v${CURL_VERSION_STRING}")

# Include & link...
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CURL_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
# N/A
