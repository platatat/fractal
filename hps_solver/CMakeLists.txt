cmake_minimum_required(VERSION 3.5)

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src/")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions(-Wfatal-errors)

project(mandelbrot CXX)

add_executable(mandelbrot
    main.cpp
    application.cpp
    bignum.cpp
    renderer.cpp
    sdl_input_controller.cpp
    tile.cpp
    tile_manager.cpp
    tile_solver.cpp
    tile_client.cpp
    tile_server.cpp
    tile_request_heap.cpp
    socket_util.cpp
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

pkg_check_modules(SDL2IMAGE REQUIRED SDL2_image)
target_include_directories(mandelbrot PRIVATE ${SDL2IMAGE_INCLUDE_DIRS})
target_link_libraries(mandelbrot ${SDL2IMAGE_LIBRARIES})

pkg_check_modules(SDL2TTF REQUIRED SDL2_ttf)
target_include_directories(mandelbrot PRIVATE ${SDL2TTF_INCLUDE_DIRS})
target_link_libraries(mandelbrot ${SDL2TTF_LIBRARIES})
