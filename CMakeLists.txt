cmake_minimum_required(VERSION 3.8)
project(untitled)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES "main.cpp" "../3dbigproject/soil-master/src/*.c")

add_executable(untitled1 ${SOURCES})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(untitled1 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
#inspired from https://stackoverflow.com/questions/9460242/how-to-compile-glut-opengl-project-with-cmake-and-kdevelop-in-linux
