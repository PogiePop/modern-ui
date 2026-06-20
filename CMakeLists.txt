cmake_minimum_required(VERSION 3.14)

project(ModernUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)




add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

file(GLOB_RECURSE modern_ui_src src/*.cpp)
add_library(modernui STATIC ${modern_ui_src})
target_include_directories(modernui PUBLIC 
    src
    external/glad/include
    external/stb
    external/glfw/include
    external/glm-0.9.8.5/glm
)

link_directories(external/glfw/lib-vc2022)

target_link_libraries(modernui PUBLIC glfw3 glad opengl32)

add_executable(exp main.cpp)

target_link_libraries(exp PUBLIC modernui)