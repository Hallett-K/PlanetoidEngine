cmake_minimum_required(VERSION 3.12.0)

project(PlanetoidEngine)

add_subdirectory(Core)
add_subdirectory(Editor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)