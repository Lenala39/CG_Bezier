##############################################################################

project(BezierSubdiv)
cmake_minimum_required(VERSION 2.8)


##############################################################################
# paths
##############################################################################

set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})


##############################################################################
# dependencies
##############################################################################

# dependencies
find_package(OpenGL REQUIRED)


##############################################################################
# compiler flags
##############################################################################

if((UNIX OR APPLE) AND CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations")
endif()

if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_USE_MATH_DEFINES -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS")
endif()


##############################################################################
# build type
##############################################################################

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
endif()


##############################################################################
# GLFW
##############################################################################

set(GLFW_SOURCE_DIR  "external/glfw")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

add_subdirectory(external/glfw)

include_directories(external/glfw/include
                    external/glfw/deps)


##############################################################################
# GLEW
##############################################################################

include_directories(external/glew/include)
add_definitions(-DGLEW_STATIC)
add_library(glew STATIC
            external/glew/src/glew.c
            external/glew/include)
target_link_libraries(glew ${GLFW_LIBRARIES})


##############################################################################
# imgui
##############################################################################

include_directories(external/imgui)
add_subdirectory(external/imgui)


##############################################################################
# Eigen
##############################################################################

set(EIGEN_SOURCE_DIR "external/eigen")
include_directories(${EIGEN_SOURCE_DIR})


##############################################################################
# pmp
##############################################################################

include_directories(external)
add_subdirectory(external/pmp)


##############################################################################
# source code directories
##############################################################################

add_definitions(-DDATA_PATH="${CMAKE_SOURCE_DIR}/models/")
add_definitions(-DSHADER_PATH="${CMAKE_SOURCE_DIR}/external/pmp/gl/")

include(AddFileDependencies)
include_directories(${PROJECT_SOURCE_DIR}/src/)
include_directories(/usr/local/include/)
include_directories(${OPENGL_INCLUDE_DIRS})

add_subdirectory(src)

##############################################################################
# documentation
##############################################################################

find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
endif(DOXYGEN_FOUND)


##############################################################################

