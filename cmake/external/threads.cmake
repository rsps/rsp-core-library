# -------------------------------------------------------------------------------------------------------------- #
# C++ std::thread
# @see https://en.cppreference.com/w/cpp/thread/thread
#
# @see https://cmake.org/cmake/help/latest/module/FindThreads.html
# -------------------------------------------------------------------------------------------------------------- #

include_guard(GLOBAL)

# Debug
message(VERBOSE "Installing STD Threads")

# Pre-configuration
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)

# Debug
message(STATUS "Found Threads")

# Include & link...
target_link_libraries(${PROJECT_NAME} Threads::Threads)

# -------------------------------------------------------------------------------------------------------------- #

# Configure build options
list(APPEND BUILD_OPTIONS "-DUSE_STD_THREAD")
