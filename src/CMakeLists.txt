file(GLOB_RECURSE SRC_FILES graphics/*.cpp posix/*.cpp logging/*.cpp utils/*.cpp application/*.cpp)
target_sources("rsp-core-lib" 
    PRIVATE 
        version.cpp
        ${SRC_FILES}
        #${UTIL_FILES}
        #filesystem/file1.cpp
        #graphics/objects/Rect.cpp
        #graphics/objects/Point.cpp
        #graphics/objects/Pen.cpp
        #graphics/objects/Colour.cpp
        #graphics/FramebufferCanvas.cpp
)

