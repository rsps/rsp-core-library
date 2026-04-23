# -------------------------------------------------------------------------------------------------------------- #
# Doctest
# @see https://github.com/doctest/doctest
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

CPMAddPackage(
    NAME doctest
    GITHUB_REPOSITORY doctest/doctest
    VERSION 2.5.2
)

if(doctest_ADDED)
    # Set include dir property because doctest::doctest doesn't export it?
    target_include_directories(doctest
        INTERFACE
            $<BUILD_INTERFACE:${doctest_SOURCE_DIR}/doctest>
    )

    # For doctest_discover_tests()
    include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")
endif()

