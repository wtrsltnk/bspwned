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
        error.cpp
        error.h
        glad.c
        include/stb_image.h
        include/stb_truetype.h
        interfaces.h
        lightmap.cpp
        lightmap.h
        main.cpp
        math3d.h
        opengl.h
        physics.cpp
        physics.h
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
        stringcompare.cpp
        stringcompare.h
        targa.cpp
        textureloader.cpp
        textureloader.h
        tokenizer.cpp
        tokenizer.h
        types.h
        vector3.cpp
        vector3.h
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
)

