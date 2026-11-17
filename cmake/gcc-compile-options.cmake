# GCC strict compile options
#
# @see https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html
#
if (NOT DEFINED RSP_GCC_STRICT_COMPILE_OPTIONS)
    set(RSP_GCC_STRICT_COMPILE_OPTIONS
        -pedantic
        -Wall
        -Wextra
        -Wold-style-cast
        -Wcast-align
        -Wcast-qual
        -Wctor-dtor-privacy
        -Wdisabled-optimization
        -Wformat=2
        -Winit-self
        -Wlogical-op
        -Wmissing-declarations
        -Wmissing-include-dirs
        -Wnoexcept
        -Woverloaded-virtual
        -Wsign-conversion
        -Wsign-promo
        -Wstrict-null-sentinel
        -Wundef
        -Wno-unused
        -Wshadow
        -Weffc++
    )
    if(RSP_CORE_LIB_WARNINGS_AS_ERRORS)
        list(APPEND RSP_GCC_STRICT_COMPILE_OPTIONS -Werror)
    endif()
endif()
