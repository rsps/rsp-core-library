file(GLOB_RECURSE SRC_FILES *.cpp)
target_sources("rsp-core-lib-test" PRIVATE
    ${SRC_FILES}
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/helpers/testImages 
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/helpers/fonts 
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../)

