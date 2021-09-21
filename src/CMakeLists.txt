file(GLOB_RECURSE SRC_FILES graphics/*.cpp filesystem/*.cpp logging/*.cpp utils/*.cpp)
target_sources("rsp-core-lib" 
    PRIVATE 
        version.cpp
        ${SRC_FILES}
        #filesystem/file1.cpp
        #graphics/objects/Rect.cpp
        #graphics/objects/Point.cpp
        #graphics/objects/Pen.cpp
        #graphics/objects/Colour.cpp
        #graphics/FramebufferCanvas.cpp
)
