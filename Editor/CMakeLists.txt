cmake_minimum_required(VERSION 3.12.0)
project(PlanetoidEngine-Editor VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(PlanetoidEngine-Editor src/Starship.cpp)
include_directories(include)

target_link_libraries(PlanetoidEngine-Editor
    PRIVATE
        PlanetoidEngine-Core
)

add_compile_definitions(PE_WINDOWS)