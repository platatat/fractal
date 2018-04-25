cmake_minimum_required(VERSION 3.5)

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src/")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(mandelbrot CXX)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=gnu++11")
endif()

add_executable(mandelbrot
    application.cpp
    bignum.cpp
    main.cpp
    renderer.cpp
    tile.cpp
    tile_manager.cpp
    tile_solver.cpp
)

find_package(PkgConfig REQUIRED)

target_link_libraries(mandelbrot ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} gmp)

pkg_check_modules(CAIRO REQUIRED cairo)
target_include_directories(mandelbrot PRIVATE ${CAIRO_INCLUDE_DIRS})
target_link_libraries(mandelbrot ${CAIRO_LIBRARIES})

pkg_check_modules(SDL2 REQUIRED sdl2)
target_include_directories(mandelbrot PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(mandelbrot ${SDL2_LIBRARIES})
