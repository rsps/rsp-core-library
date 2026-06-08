# GCC strict compile options
#
# @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Option-Index.html
# @see https://gcc.gnu.org/onlinedocs/
#
if (NOT DEFINED RSP_GCC_STRICT_COMPILE_OPTIONS)
    set(RSP_GCC_STRICT_COMPILE_OPTIONS

        # Issue all the warnings demanded by strict ISO C and ISO C++.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-pedantic-1
        #
        -pedantic

        # Enables all the warnings about constructions...
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wall
        #
        -Wall

        # Enables some extra warning flags that are not enabled by -Wall.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wextra
        #
        -Wextra

        # Warn if an old-style (C-style) cast to a non-void type is used within a C++ program.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/C_002b_002b-Dialect-Options.html#index-Wold-style-cast
        #
        -Wold-style-cast

        # Warn whenever a pointer is cast such that the required alignment of the target is increased.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wcast-align
        #
        -Wcast-align

        # Warn whenever a pointer is cast so as to remove a type qualifier from the target type.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wcast-qual
        #
        -Wcast-qual

        # Warn when a class seems unusable because all the constructors or destructors in that class
        # are private, and it has neither friends nor public static member functions.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/C_002b_002b-Dialect-Options.html#index-Wctor-dtor-privacy
        #
        -Wctor-dtor-privacy

        # Warn if a requested optimization pass is disabled.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wdisabled-optimization
        #
        -Wdisabled-optimization

        # Check calls to printf and scanf, etc., to make sure that the arguments supplied have types
        # appropriate to the format string specified.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wformat
        #
        -Wformat=2

        # Warn about uninitialized variables that are initialized with themselves.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Winit-self
        #
        -Winit-self

        # Warn about suspicious uses of logical operators in expressions.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wlogical-op
        #
        -Wlogical-op

        # Warn if a global function is defined without a previous declaration.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wmissing-declarations
        #
        -Wmissing-declarations

        # Warn if a user-supplied include directory does not exist.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wmissing-include-dirs
        #
        -Wmissing-include-dirs

        # Warn when a noexcept-expression evaluates to false because of a call to a function that
        # does not have a non-throwing exception specification.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/C_002b_002b-Dialect-Options.html#index-Wnoexcept
        #
        -Wnoexcept

        # Warn when a function declaration hides virtual functions from a base class.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/C_002b_002b-Dialect-Options.html#index-Woverloaded-virtual
        #
        -Woverloaded-virtual

        # Warn if anything is declared more than once in the same scope.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wredundant-decls
        #
        # -Wredundant-decls

        # Warn for implicit conversions that may change the sign of an integer value, like assigning
        # a signed integer expression to an unsigned integer variable.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wsign-conversion
        #
        -Wsign-conversion

        # Warn when a comparison between signed and unsigned values could produce an incorrect result
        # when the signed value is converted to unsigned.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wsign-compare
        #
        #-Wsign-compare # (Enabled by -Wall)

        # Warn when overload resolution chooses a promotion from unsigned or enumerated type to a
        # signed type, over a conversion to an unsigned type of the same size.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/C_002b_002b-Dialect-Options.html#index-Wsign-promo
        #
        -Wsign-promo

        # Warn about the use of an uncasted NULL as sentinel.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/C_002b_002b-Dialect-Options.html#index-Wstrict-null-sentinel
        #
        -Wstrict-null-sentinel

        # Warns about cases where the compiler optimizes based on the assumption that signed overflow does not occur.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wstrict-overflow
        #
        # -Wstrict-overflow=3 # (-Wall only includes Wstrict-overflow=1) // disabled because it causes false positives

        # Warn whenever a switch statement does not have a default case.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wswitch-default
        #
        # -Wswitch-default

        # Warn if an undefined identifier is evaluated in an #if directive.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wundef
        #
        -Wundef

        # All the -Wunused options combined.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wno-unused
        #
        -Wno-unused

        # Warn whenever a local variable or type declaration shadows another.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wshadow
        #
        -Wshadow

        # Warn about violations of the style guidelines from Scott Meyers’ Effective C++ series
        # of books.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/C_002b_002b-Dialect-Options.html#index-Weffc_002b_002b
        #
        -Weffc++

        # Make all warnings into errors.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Werror
        #
        -Werror

        # Print (on standard error output) the commands executed to run the stages of compilation.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Overall-Options.html#index-v
        #
        #-v

        # Dump all macro definitions, at the end of preprocessing, in addition to normal output.
        #
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Preprocessor-Options.html#index-dD
        # @see https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Developer-Options.html#index-dD-1
        #
        #-dD
    )
endif()
