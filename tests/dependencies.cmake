include_guard(GLOBAL)

function(rsp_core_test_add_dependencies ATARGET)
    message(NOTICE "Installing test dependencies for ${ATARGET}")

    include(FetchContent)

    FetchContent_Declare(doctest
        GIT_REPOSITORY https://github.com/doctest/doctest
        GIT_TAG        v2.5.2
        GIT_SHALLOW    TRUE
    )
    set(DOCTEST_NO_INSTALL ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(doctest)
    include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")  # for doctest_discover_tests()

    target_link_libraries(${ATARGET}
        PRIVATE
            rsp-core-lib
            doctest::doctest
    )

endfunction()
