file(GLOB_RECURSE GRPH_FILES graphics/*.cpp)
file(GLOB_RECURSE UTIL_FILES utils/*.cpp)
target_sources("rsp-core-lib" 
    PRIVATE 
        version.cpp
        ${GRPH_FILES}
        ${UTIL_FILES}
        #filesystem/file1.cpp
        #graphics/objects/Rect.cpp
        #graphics/objects/Point.cpp
        #graphics/objects/Pen.cpp
        #graphics/objects/Colour.cpp
        #graphics/FramebufferCanvas.cpp
)
