include(FetchContent)
find_package(Git REQUIRED)

# Include external projects Doctest from github.
FetchContent_Declare(
    DocTest
    GIT_REPOSITORY "https://github.com/onqtam/doctest"
    GIT_TAG "v2.4.9"
)
FetchContent_MakeAvailable(DocTest)

FetchContent_Declare(
    lighttpd_src
    GIT_REPOSITORY "https://github.com/lighttpd/lighttpd1.4"
    GIT_TAG "lighttpd-1.4.66"
)

FetchContent_GetProperties(lighttpd_src)
if(NOT lighttpd_POPULATED)
    FetchContent_Populate(lighttpd_src)
endif()

add_custom_target(lighttpd
    COMMENT "Building lighttpd..."
    BYPRODUCTS ${lighttpd_src_BINARY_DIR}/build/lighttpd
    COMMAND cmake -G${CMAKE_GENERATOR} -DWITH_OPENSSL=ON ${lighttpd_src_SOURCE_DIR} && cmake --build .
    WORKING_DIRECTORY ${lighttpd_src_BINARY_DIR}
    SOURCES ${lighttpd_src_SOURCE_DIR}/src/lemon.c
    VERBATIM
)

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/webserver/lighttpd.conf.in"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/webserver/lighttpd.conf"
)

enable_testing()

set (TEST_BINARY "rsp-core-lib-test")

add_executable(${TEST_BINARY})
add_dependencies(${TEST_BINARY} doctest lighttpd)
target_include_directories(${TEST_BINARY}
    PUBLIC
        ${doctest_SOURCE_DIR}/doctest
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers
)

if (NOT RELEASE_BUILD)
target_compile_options(${TEST_BINARY} PRIVATE 
    -DGFX_FPS=20
    -g
    -O0
    )
endif()

#get_target_property(INCLUDE_DIRS "rsp-core-lib-test" INCLUDE_DIRECTORIES)
#message(STATUS "IncDirs: ${INCLUDE_DIRS}")

target_link_libraries (${TEST_BINARY}
    rsp-core-lib
    Threads::Threads
)
#target_link_libraries(demo ${FREETYPE_LIBRARIES})

add_test("tests" ${TEST_BINARY})


file(GLOB_RECURSE SRC_FILES *.cpp)
target_sources(${TEST_BINARY} PRIVATE
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
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/webserver/cgi
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/webserver/)

    
install(TARGETS ${TEST_BINARY} DESTINATION tgm/)
