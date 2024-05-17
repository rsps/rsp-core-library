file(GLOB_RECURSE SRC_FILES
        application/*.cpp
        compression/*.cpp
        exceptions/*.cpp
        json/*.cpp
        localization/*.cpp
        logging/*.cpp
        messaging/*.cpp
        network/*.cpp
        posix/*.cpp
        security/*.cpp
        utils/*.cpp
)

if (GFX)
    file(GLOB_RECURSE GFX_SRC_FILES graphics/*.cpp)
endif ()

target_sources("rsp-core-lib"
    PRIVATE
        version.cpp
        ${SRC_FILES}
        ${GFX_SRC_FILES}
)
