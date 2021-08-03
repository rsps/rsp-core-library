file(GLOB_RECURSE SRC_FILES graphics/*.cpp)
target_sources("rsp-core-lib" 
    PRIVATE 
        version.cpp
        ${SRC_FILES}
        #filesystem/file1.cpp
        #graphics/objects/Rect.cpp
        #graphics/objects/Point.cpp
        #graphics/objects/Pen.cpp
        #graphics/objects/Color.cpp
        #graphics/FramebufferCanvas.cpp
)
