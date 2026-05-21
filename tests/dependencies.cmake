include_guard(GLOBAL)

function(rsp_core_test_add_dependencies ATARGET)
    message(NOTICE "Installing test dependencies for ${ATARGET}")

    include(FetchContent)

    FetchContent_Declare(doctest
        GIT_REPOSITORY https://github.com/doctest/doctest
        GIT_TAG        v2.5.2
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(doctest)
    include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")  # for doctest_discover_tests()

    # Use ExternalProject to isolate the lighttpd build
    include(ExternalProject)
    ExternalProject_Add(lighttpd
        GIT_REPOSITORY  https://github.com/lighttpd/lighttpd1.4
        # GIT_REPOSITORY https://git.lighttpd.net/lighttpd/lighttpd1.4.git # Original git repo
        GIT_TAG         lighttpd-1.4.82
        GIT_SHALLOW     TRUE
        TMP_DIR         "${FETCHCONTENT_BASE_DIR}/lighttpd-tmp"
        STAMP_DIR       "${FETCHCONTENT_BASE_DIR}/lighttpd-stamp"
        SOURCE_DIR      "${FETCHCONTENT_BASE_DIR}/lighttpd-src"
        BINARY_DIR      "${FETCHCONTENT_BASE_DIR}/lighttpd-build"
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_ARGS      --log-level=WARNING -Wno-deprecated -DCMAKE_BUILD_TYPE=Release -DWITH_OPENSSL=ON
        INSTALL_COMMAND ""  # skip install step
    )

    add_dependencies(${ATARGET} lighttpd)

    target_link_libraries(${ATARGET}
        PUBLIC
            rsp-core-lib
            doctest::doctest
    )

endfunction()
