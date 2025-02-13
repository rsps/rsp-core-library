# -------------------------------------------------------------------------------------------------------------- #
# Setup Project version
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "setup-version module included")

# Read version from root version file
include("rsp/version")
version_from_file(
    FILE "${CMAKE_CURRENT_SOURCE_DIR}/VERSION"
    OUTPUT version
    EXIT_ON_FAILURE
)