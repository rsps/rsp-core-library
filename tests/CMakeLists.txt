file(GLOB_RECURSE SRC_FILES unit/*.cpp)
target_sources("rsp-core-lib-test" PRIVATE 
    test-main.cpp
    ${SRC_FILES}
    #unit/test-version.cpp
    #unit/graphics/test-rect.cpp
    #unit/graphics/test-color.cpp
)
