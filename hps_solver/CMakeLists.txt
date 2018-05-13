cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src/")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (HPS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
    add_definitions(-DHPS)
endif ()

add_definitions(-Wfatal-errors)

project(mandelbrot C CXX)

set(MANDELBROT_SOURCES
    main.cpp
    bignum.cpp
    #fpga_solver.cpp
    tile.cpp
    cpu_solver.cpp
    tile_client.cpp
    tile_server.cpp
    tile_request_heap.cpp
    socket_util.cpp
    solver.cpp
)

if (NOT HPS)
    set(MANDELBROT_SOURCES
        ${MANDELBROT_SOURCES}
        application.cpp
        renderer.cpp
        sdl_input_controller.cpp
        tile_manager.cpp
    )
endif()

add_executable(mandelbrot ${MANDELBROT_SOURCES})

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(mandelbrot
        ${CMAKE_THREAD_LIBS_INIT}
        ${OPENGL_LIBRARIES} 
        ${GLUT_LIBRARY}  
        gmp)

if (NOT HPS)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(mandelbrot ${SDL2_LIBRARIES})

    pkg_check_modules(SDL2IMAGE REQUIRED SDL2_image)
    include_directories(${SDL2IMAGE_INCLUDE_DIRS})
    target_link_libraries(mandelbrot ${SDL2IMAGE_LIBRARIES})

    pkg_check_modules(SDL2TTF REQUIRED SDL2_ttf)
    include_directories(${SDL2TTF_INCLUDE_DIRS})
    target_link_libraries(mandelbrot ${SDL2TTF_LIBRARIES})
endif()
