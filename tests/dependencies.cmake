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

    find_package(lighttpd 1.4.74 REQUIRED)

    target_compile_definitions(${ATARGET}
        PRIVATE
            LIGHTTPD_EXECUTABLE="${lighttpd_EXECUTABLE}"
            LIGHTTPD_MODULES_DIR="${lighttpd_MODULES_DIR}"
    )

    target_link_libraries(${ATARGET}
        PUBLIC
            rsp-core-lib
            doctest::doctest
    )

endfunction()
