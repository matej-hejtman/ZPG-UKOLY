# Author: Matt Hejtman, login: HEJ0094
cmake_minimum_required(VERSION 3.16)
project(ZPG CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB HEADERS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.h)
add_executable(ZPG ${SOURCES} ${HEADERS})

# All paths are relative to the project root - no absolute paths
target_include_directories(ZPG PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/assets/models
    ${CMAKE_SOURCE_DIR}/libs/glad/include
    ${CMAKE_SOURCE_DIR}/libs/glm
    ${CMAKE_SOURCE_DIR}/libs/stb)

if(WIN32)
    # Prebuilt GLFW for Visual Studio is expected in libs/glfw
    target_include_directories(ZPG PRIVATE ${CMAKE_SOURCE_DIR}/libs/glfw/include)
    target_link_directories(ZPG PRIVATE ${CMAKE_SOURCE_DIR}/libs/glfw/lib-vc2022)
    # Prebuilt Assimp for Visual Studio is expected in libs/assimp
    target_include_directories(ZPG PRIVATE ${CMAKE_SOURCE_DIR}/libs/assimp/include)
    target_link_directories(ZPG PRIVATE ${CMAKE_SOURCE_DIR}/libs/assimp/lib)
    target_link_libraries(ZPG PRIVATE glfw3.lib opengl32.lib assimp-vc143-mt.lib)
    set_property(TARGET ZPG PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ZPG)
else()
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(assimp REQUIRED)   # sudo apt install libassimp-dev
    target_link_libraries(ZPG PRIVATE glfw OpenGL::GL assimp::assimp ${CMAKE_DL_LIBS})
    target_compile_options(ZPG PRIVATE -Wall -Wextra)
endif()

# Copy shaders and models next to the executable so relative paths work there too
add_custom_command(TARGET ZPG POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ZPG>/assets)
