# -------------------------------------------------------------------------------------------------------------- #
# Dependencies
# -------------------------------------------------------------------------------------------------------------- #

include_guard()

macro(install_dependencies)
    message(VERBOSE "Installing Dependencies for ${PROJECT_NAME}")

    # Avoid building tests for dependencies...
    set(BUILD_TESTING off)

    # Add dependencies here...
    message(VERBOSE "    N/A")
endmacro()
safeguard_properties("install_dependencies" "BUILD_TESTING")
