project('fractal', 'cpp')

add_global_arguments('-std=c++14', language : 'cpp')
add_global_arguments('-Wfatal-errors', language : 'cpp')

deps = [
    dependency('threads'),
    dependency('sdl2'),
    dependency('gmpxx')
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
