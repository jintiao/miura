cmake_minimum_required (VERSION 2.8)


project (miura)


set(INCLUDE_DIR src)
set(LINK_LIBS "")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")


# GLEW
set(INCLUDE_DIR ${INCLUDE_DIR} 3rd/glew/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGLEW_STATIC")


# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(3rd/glfw)
set(LINK_LIBS ${LINK_LIBS} glfw)


# OpenGL
find_package(OpenGL REQUIRED)
set(INCLUDE_DIR ${INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
set(LINK_LIBS ${LINK_LIBS} ${OPENGL_gl_LIBRARY})


# miura
set(SRC_GL 3rd/glew/src/glew.c src/GLHeader.h)
SOURCE_GROUP(gl FILES ${SRC_GL})
set(SRC_MAIN src/main.cpp src/Game.cpp src/Game.h )
SOURCE_GROUP(src FILES ${SRC_MAIN})
add_executable(miura ${SRC_MAIN} ${SRC_GL})
target_link_libraries(miura ${LINK_LIBS})
include_directories(${INCLUDE_DIR})
