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
    # For doctest_discover_tests()
    include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")
endif()

