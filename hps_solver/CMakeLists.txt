cmake_minimum_required(VERSION 3.5)

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src/")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(mandelbrot CXX)

add_executable(mandelbrot
    application.cpp
    bignum.cpp
    main.cpp
    renderer.cpp
    tile.cpp
    tile_manager.cpp
    tile_solver.cpp
)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(mandelbrot ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
