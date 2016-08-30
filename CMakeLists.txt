cmake_minimum_required (VERSION 3.6)


project (miura)




set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(3rd/glfw)


find_package(OpenGL REQUIRED)


set(INC_DIR )
set(SRC src/main.cpp src/Game.cpp 3rd/glew/src/glew.c)


set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -DGLEW_STATIC")


add_executable(miura ${SRC})
target_link_libraries(miura glfw)
target_link_libraries(miura ${OPENGL_gl_LIBRARY})
include_directories(src 3rd/glew/include ${OPENGL_INCLUDE_DIR})
