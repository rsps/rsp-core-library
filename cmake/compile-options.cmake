# Strict compile options per compiler.
#
# The Clang list is the subset of the GCC list that Clang supports.
#
# https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html
# https://clang.llvm.org/docs/DiagnosticsReference.html

set(RSP_CORE_LIB_STRICT_COMPILE_OPTIONS_GCC
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

set(RSP_CORE_LIB_STRICT_COMPILE_OPTIONS_CLANG
    -pedantic
    -Wall
    -Wextra
    -Wold-style-cast
    -Wcast-align
    -Wcast-qual
    -Wformat=2
    -Winit-self
    -Wmissing-declarations
    -Woverloaded-virtual
    -Wsign-conversion
    -Wundef
    -Wno-unused
    -Wno-unused-parameter
    -Wshadow
)

if(RSP_CORE_LIB_WARNINGS_AS_ERRORS)
    list(APPEND RSP_CORE_LIB_STRICT_COMPILE_OPTIONS_GCC -Werror)
    list(APPEND RSP_CORE_LIB_STRICT_COMPILE_OPTIONS_CLANG -Werror)
endif()
