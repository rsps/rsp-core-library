file(GLOB_RECURSE SRC_FILES *.cpp)
target_sources("rsp-core-lib" 
    PRIVATE 
        ${SRC_FILES}
)
