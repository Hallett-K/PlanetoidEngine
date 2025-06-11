cmake_minimum_required(VERSION 3.12.0)
project(PlanetoidEngine-Core VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(ext/glfw)
add_subdirectory(ext/freetype)
add_subdirectory(ext/box2d-2.4.1)
add_library(glad ext/glad/glad.c)
add_library(imgui ext/imgui/imgui.cpp ext/imgui/imgui_tables.cpp ext/imgui/imgui_draw.cpp ext/imgui/imgui_widgets.cpp ext/imgui/imgui_demo.cpp ext/imgui/imgui_impl_glfw.cpp ext/imgui/imgui_impl_opengl3.cpp ext/imgui/imgui_stdlib.cpp)

include_directories(include ext/freetype/include ext/imgui ext/box2d-2.4.1/include)

add_library(PlanetoidEngine-Core SHARED 
    src/Core/InputManager.cpp src/Core/Application.cpp src/Core/Window.cpp src/Core/Extensions.cpp src/Core/UUID.cpp src/Core/FileIO.cpp src/Core/Log.cpp src/Core/AssetStore.cpp src/Core/TimelineManager.cpp
    src/VirtualFileSystem/VirtualFileSystem.cpp
    src/Scenes/Scene.cpp src/Scenes/SceneManager.cpp src/Scenes/Entity.cpp src/Scenes/Components.cpp
    src/Renderer/Shader.cpp src/Renderer/Texture.cpp src/Renderer/Framebuffer.cpp src/Renderer/Renderer.cpp
    src/ScriptEngine/ScriptDictionary.cpp src/ScriptEngine/ScriptComponent.cpp src/ScriptEngine/ScriptAPI.cpp)

target_include_directories(PlanetoidEngine-Core PUBLIC include ext/imgui)
target_link_libraries(PlanetoidEngine-Core glfw opengl32 glad freetype imgui box2d)

add_compile_definitions(PE_WINDOWS PE_DLL)