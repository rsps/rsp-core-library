# -------------------------------------------------------------------------------------------------------------- #
# Doctest
# @see https://github.com/doctest/doctest
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing Doctest")

set(DOCTEST_VERSION "2.4.11")

# NOTE: This can yield a strange DEPRECATION WARNING, which has yet to be resolved by the provider!
# @see https://github.com/doctest/doctest/issues/893
# @see https://github.com/doctest/doctest/issues/854

CPMAddPackage(
    NAME "DocTest"
    GITHUB_REPOSITORY "onqtam/doctest"
    GIT_TAG "v${DOCTEST_VERSION}"
    # VERSION "${DOCTEST_VERSION}"
)

# Debug
message(STATUS "Using Doctest v${DOCTEST_VERSION}")

# Include & link...
target_include_directories(${TEST_BINARY}
    PUBLIC
        ${doctest_SOURCE_DIR}/doctest
)

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
# N/A
