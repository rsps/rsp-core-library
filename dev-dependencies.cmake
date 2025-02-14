# -------------------------------------------------------------------------------------------------------------- #
# Development Dependencies
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Include regular dependencies
include("dependencies.cmake")

macro(install_dev_dependencies)
    message(NOTICE "${COLOR_WHITE}Installing Development Dependencies for${RESTORE} ${PROJECT_NAME}")

    # -------------------------------------------------------------------------------------------------------------- #

    include("external/doctest")
    include("external/lighttpd")

    # -------------------------------------------------------------------------------------------------------------- #

    # ...etc

endmacro()
safeguard_properties("install_dev_dependencies" "")
