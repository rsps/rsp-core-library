file(GLOB_RECURSE SRC_FILES *.cpp)
target_sources("rsp-core-lib-test" PRIVATE
    test-main.cpp
    ${SRC_FILES}
    #unit/test-version.cpp
    #unit/graphics/test-rect.cpp
    #unit/graphics/test-colour.cpp
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/unit/graphics/testImages 
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/unit/graphics/fonts 
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

