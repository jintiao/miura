cmake_minimum_required (VERSION 2.8)


# global variables
set(PROJECT_NAME "miura")
set(INCLUDE_DIR "")
set(LINK_LIBS "")
set(CMAKE_CXX_FLAGS "-std=c++11")


# GLEW
set(INCLUDE_DIR ${INCLUDE_DIR} 3rd/glew/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGLEW_STATIC")


# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(3rd/glfw)
set(LINK_LIBS ${LINK_LIBS} glfw)


# GLM
set(INCLUDE_DIR ${INCLUDE_DIR} 3rd/glm)


# OpenGL
find_package(OpenGL REQUIRED)
set(INCLUDE_DIR ${INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
set(LINK_LIBS ${LINK_LIBS} ${OPENGL_gl_LIBRARY})


# miura
project (${PROJECT_NAME})

## include
set(INCLUDE_DIR ${INCLUDE_DIR} src)

## source
set(SRC_OCEAN 	src/OceanObject.cpp src/OceanObject.h
				src/WaveSimulator.cpp src/WaveSimulator.h
				src/OceanShader.cpp
				src/Environment.h)
SOURCE_GROUP(ocean FILES ${SRC_OCEAN})
set(SRC_RENDERING src/main.cpp 
				src/Renderer.cpp src/Renderer.h
				src/Camera.cpp src/Camera.h)
SOURCE_GROUP(rendering FILES ${SRC_RENDERING})
set(SRC_BORING src/GlobalHeader.h
				src/MyMath.cpp src/MyMath.h
				3rd/glew/src/glew.c)
SOURCE_GROUP(boring FILES ${SRC_BORING})

## build
add_executable(${PROJECT_NAME} ${SRC_OCEAN} ${SRC_RENDERING} ${SRC_BORING})
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})
include_directories(${INCLUDE_DIR})

## compiler
if (MSVC)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W4 -EHsc -D_USE_MATH_DEFINES -D_WIN")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif (MSVC)
