cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(Bspwned)

find_package(OpenGL REQUIRED)

include(cmake/CPM.cmake)
include(cmake/Dependencies.cmake)

add_executable(Bspwned)

target_sources(Bspwned
    PUBLIC
        camera.cpp
        camera.h
        entitymanager.cpp
        entitymanager.h
        glad.c
        include/stb_image.h
        include/stb_truetype.h
        interfaces.h
        lightmap.cpp
        lightmap.h
        main.cpp
        opengl.h
        resources.cpp
        resources.h
        shadermanager.cpp
        shadermanager.h
        skyrenderer.cpp
        skyrenderer.h
        staticrenderer.cpp
        staticrenderer.h
        stb_image.cpp
        stb_truetype.cpp
        textureloader.cpp
        textureloader.h
        tokenizer.cpp
        tokenizer.h
        types.h
        wadloader.cpp
        wadloader.h
        worldloader.cpp
        worldloader.h
        worldrenderer.cpp
        worldrenderer.h
)

target_include_directories(Bspwned
    PRIVATE include
)

target_compile_features(Bspwned
    PRIVATE cxx_std_17
)

target_link_libraries(Bspwned
    PUBLIC
        ${OPENGL_LIBRARIES}
        glfw
        spdlog
        glm
)

