# Author: Matěj Hejtman, login: HEJ0094
cmake_minimum_required(VERSION 3.16)
project(ZPG C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Knihovny - vse je primo v libs/, nic se nestahuje ani neinstaluje.
#   libs/glad, libs/glm, libs/stb  ... header-only / jeden zdrojak
#   libs/glfw   (GLFW 3.4)          ... kompiluje se ze zdrojaku
#   libs/assimp (Assimp 5.4.3)      ... kompiluje se ze zdrojaku (jen OBJ import)
# ---------------------------------------------------------------------------
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# GLFW: jen knihovna, bez prikladu/testu/dokumentace
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/glfw EXCLUDE_FROM_ALL)

# Assimp: jen OBJ importer, zadne exportery/nastroje/testy, zlib z contrib
set(ASSIMP_BUILD_TESTS                    OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS             OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES                  OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL                        OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL_PDB                    OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT                      ON  CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB                     ON  CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS             OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX           OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER             ON  CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/assimp EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# Aplikace
# ---------------------------------------------------------------------------
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.h)
add_executable(ZPG ${SOURCES} ${HEADERS})

# All paths are relative to the project root - no absolute paths
target_include_directories(ZPG PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/assets/models/headers
    ${CMAKE_SOURCE_DIR}/libs/glad/include
    ${CMAKE_SOURCE_DIR}/libs/glm
    ${CMAKE_SOURCE_DIR}/libs/stb)

find_package(OpenGL REQUIRED)
target_link_libraries(ZPG PRIVATE glfw assimp OpenGL::GL ${CMAKE_DL_LIBS})

if(MSVC)
    target_compile_options(ZPG PRIVATE /W3 /utf-8)
    set_property(TARGET ZPG PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ZPG)
else()
    target_compile_options(ZPG PRIVATE -Wall -Wextra)
endif()

# Copy shaders and models next to the executable so relative paths work there too
add_custom_command(TARGET ZPG POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ZPG>/assets)
