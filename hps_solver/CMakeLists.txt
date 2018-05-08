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
    sdl_input_controller.cpp
    tile.cpp
    tile_manager.cpp
    tile_solver.cpp
    tile_client.cpp
    tile_server.cpp
)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(mandelbrot
        ${CMAKE_THREAD_LIBS_INIT}
        ${OPENGL_LIBRARIES} 
        ${GLUT_LIBRARY}  
        gmp)

pkg_check_modules(CAIRO REQUIRED cairo)
target_include_directories(mandelbrot PRIVATE ${CAIRO_INCLUDE_DIRS})
target_link_libraries(mandelbrot ${CAIRO_LIBRARIES})

pkg_check_modules(SDL2 REQUIRED sdl2)
target_include_directories(mandelbrot PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(mandelbrot ${SDL2_LIBRARIES})
