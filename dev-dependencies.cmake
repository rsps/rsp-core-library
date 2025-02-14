# -------------------------------------------------------------------------------------------------------------- #
# Development Dependencies
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Include regular dependencies
include("dependencies.cmake")

macro(install_dev_dependencies)
    message(NOTICE "${COLOR_WHITE}Installing Development Dependencies for${RESTORE} ${PROJECT_NAME}")

    # Avoid building tests for dependencies...
    set(BUILD_TESTING off)

    # Add dev-dependencies here...
    message(VERBOSE "    N/A")
endmacro()
safeguard_properties("install_dev_dependencies" "BUILD_TESTING")
