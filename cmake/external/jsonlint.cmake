# -------------------------------------------------------------------------------------------------------------- #
# JSON Lint
# @see https://github.com/Seldaek/jsonlint
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing JSON Lint")

# NOTE: This is a runtime dependency that SHOULD be installed on the system
#       sudo apt install jsonlint


find_program(jsonlint NAMES "jsonlint-php" REQUIRED DOC " RSP Core Lib - Path to JSON Lint")

# Debug
if (DEFINED "jsonlint-NOTFOUND")
    message(STATUS "JSON Lint (${COLOR_RED}${TEXT_ITALIC}not found${RESTORE})")
else ()
    message(STATUS "Using JSON Lint (${COLOR_WHITE}${TEXT_ITALIC}${jsonlint}${RESTORE})")
endif ()

# Include & link...
# N/A

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
# N/A
