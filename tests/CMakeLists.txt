include(FetchContent)
find_package(Git REQUIRED)

# Include external projects Doctest from github.
FetchContent_Declare(
    DocTest
    GIT_REPOSITORY "https://github.com/onqtam/doctest"
    GIT_TAG "v2.4.8"
)
FetchContent_MakeAvailable(DocTest)

include(ExternalProject)
ExternalProject_Add(Lighttpd
    GIT_REPOSITORY "https://github.com/lighttpd/lighttpd1.4"
    GIT_TAG "lighttpd-1.4.65"
    PREFIX "${PROJECT_BINARY_DIR}/_deps/Lighttpd"
    CMAKE_ARGS
        -DWITH_OPENSSL=ON
        -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/webserver
    TEST_COMMAND ""
)

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/webserver/lighttpd.conf.in"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/webserver/lighttpd.conf"
)

enable_testing()

add_executable("rsp-core-lib-test")
add_dependencies("rsp-core-lib-test" doctest)
target_include_directories("rsp-core-lib-test"
    PUBLIC
        ${doctest_SOURCE_DIR}/doctest
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers
)
# Disable shadow warning to silence doctest...
target_compile_options(rsp-core-lib-test PRIVATE
    ${GOOD_COMPILER_OPTIONS}
)

if (NOT RELEASE_BUILD)
target_compile_options(rsp-core-lib-test PRIVATE 
    -DGFX_FPS=100
    -g
    -O0
    )
endif()

#get_target_property(INCLUDE_DIRS "rsp-core-lib-test" INCLUDE_DIRECTORIES)
#message(STATUS "IncDirs: ${INCLUDE_DIRS}")

target_link_libraries ("rsp-core-lib-test"
    rsp-core-lib
    Threads::Threads
)
#target_link_libraries(demo ${FREETYPE_LIBRARIES})

add_test("tests" "rsp-core-lib-test")


file(GLOB_RECURSE SRC_FILES *.cpp)
target_sources("rsp-core-lib-test" PRIVATE
    ${SRC_FILES}
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/helpers/testImages 
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/helpers/fonts 
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/webserver/public
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/webserver/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/webserver/ssl
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/webserver/)

