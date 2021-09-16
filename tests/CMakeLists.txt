file(GLOB_RECURSE SRC_FILES unit/*.cpp)
target_sources("rsp-core-lib-test" PRIVATE 
    test-main.cpp
    ${SRC_FILES}
    #unit/test-version.cpp
    #unit/graphics/test-rect.cpp
    #unit/graphics/test-colour.cpp
)

file(COPY ${CMAKE_SOURCE_DIR}/tests/unit/graphics/testImages/testImageCross.bmp DESTINATION ${CMAKE_BINARY_DIR}/testImages/)
