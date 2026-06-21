cmake_minimum_required(VERSION 3.14)

project(ModernUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

# Use UTF-8 source encoding for CJK/Unicode string literals
if(MSVC)
    add_compile_options("/utf-8")
endif()




add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

file(GLOB_RECURSE modern_ui_src src/*.cpp)
add_library(modernui_static STATIC ${modern_ui_src})
target_include_directories(modernui_static PUBLIC
    src
    external/glad/include
    external/stb
    external/glfw/include
    external/glm-0.9.8.5/glm
)
link_directories(external/glfw/lib-vc2022)
target_link_libraries(modernui_static PUBLIC glfw3 glad opengl32)
add_library(modernui ALIAS modernui_static)

# DLL version (auto-export all symbols on Windows)
add_library(modernui_shared SHARED ${modern_ui_src})
set_target_properties(modernui_shared PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
target_include_directories(modernui_shared PUBLIC
    src
    external/glad/include
    external/stb
    external/glfw/include
    external/glm-0.9.8.5/glm
)
target_compile_definitions(modernui_shared PRIVATE MODERNUI_BUILD_DLL)
target_compile_definitions(modernui_shared INTERFACE MODERNUI_USE_DLL)
target_link_libraries(modernui_shared PUBLIC glfw3 glad opengl32)

# ── Install targets (for release packaging) ──
include(GNUInstallDirs)
install(TARGETS modernui_static modernui_shared
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY src/ DESTINATION include/modernui
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.cpp" EXCLUDE
)
install(DIRECTORY external/glad/include/ DESTINATION include)
install(DIRECTORY external/stb/ DESTINATION include/stb
    FILES_MATCHING PATTERN "*.h")
install(DIRECTORY external/glm-0.9.8.5/glm/ DESTINATION include/glm)
install(DIRECTORY external/glfw/include/ DESTINATION include)
install(FILES external/glfw/lib-vc2022/glfw3.dll DESTINATION bin)
install(FILES external/glfw/lib-vc2022/glfw3.lib DESTINATION lib)

# ── Demo targets use static lib ──

add_executable(exp main.cpp)
target_link_libraries(exp PUBLIC modernui_static)

# Test demos
add_executable(test_login test/login.cpp)
target_include_directories(test_login PRIVATE src external/glad/include external/stb external/glfw/include external/glm-0.9.8.5/glm)
target_link_libraries(test_login PRIVATE modernui_static glfw3 glad opengl32)

add_executable(test_profile test/profile.cpp)
target_include_directories(test_profile PRIVATE src external/glad/include external/stb external/glfw/include external/glm-0.9.8.5/glm)
target_link_libraries(test_profile PRIVATE modernui_static glfw3 glad opengl32)

add_executable(test_music test/music.cpp)
target_include_directories(test_music PRIVATE src external/glad/include external/stb external/glfw/include external/glm-0.9.8.5/glm)
target_link_libraries(test_music PRIVATE modernui_static glfw3 glad opengl32)

add_executable(test_showcase test/showcase.cpp)
target_include_directories(test_showcase PRIVATE src external/glad/include external/stb external/glfw/include external/glm-0.9.8.5/glm)
target_link_libraries(test_showcase PRIVATE modernui_static glfw3 glad opengl32)

add_executable(test_unit test/test_all.cpp)
target_include_directories(test_unit PRIVATE src external/glad/include external/stb external/glfw/include external/glm-0.9.8.5/glm)
target_link_libraries(test_unit PRIVATE modernui_static glfw3 glad opengl32)

add_executable(test_simple test/test_simple.cpp)
target_include_directories(test_simple PRIVATE src)

add_executable(test_hello test/test_hello.cpp)
target_include_directories(test_hello PRIVATE src external/glad/include external/stb external/glfw/include external/glm-0.9.8.5/glm)
target_link_libraries(test_hello PRIVATE modernui_static glfw3 glad opengl32)

add_executable(test_demo test/demo.cpp)
target_include_directories(test_demo PRIVATE src external/glad/include external/stb external/glfw/include external/glm-0.9.8.5/glm)
target_link_libraries(test_demo PRIVATE modernui_static glfw3 glad opengl32)

add_executable(test_cjk test/test_cjk.cpp)
target_include_directories(test_cjk PRIVATE src external/glad/include external/stb external/glfw/include external/glm-0.9.8.5/glm)
target_link_libraries(test_cjk PRIVATE modernui_static glfw3 glad opengl32)