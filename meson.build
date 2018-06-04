project('fractal', 'cpp')

add_global_arguments('-std=c++14', language : 'cpp')
add_global_arguments('-Wfatal-errors', language : 'cpp')

cc = meson.get_compiler('cpp')

deps = [
    dependency('threads'),
    dependency('sdl2'),
    dependency('SDL2_ttf'),
    cc.find_library('gmp'),
    cc.find_library('gmpxx')
]

srcs = [
    'application.cpp',
    'bignum.cpp',
    'cpu_solver.cpp',
    'main.cpp',
    'renderer.cpp',
    'sdl_input_controller.cpp',
    'socket_util.cpp',
    'solver.cpp',
    'tile.cpp',
    'tile_client.cpp',
    'tile_manager.cpp',
    'tile_request_heap.cpp',
    'tile_server.cpp',
    'viewport.cpp'
]

executable('fractal', sources: srcs, dependencies: deps)
