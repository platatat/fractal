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
    'balancer.cpp',
    'main.cpp',
    'renderer.cpp',
    'tile_client.cpp',
    'tile_manager.cpp',
    'tile_request_heap.cpp',
    'tile_server.cpp',
    'viewport.cpp',
    'worker.cpp',
    'input/controller.cpp',
    'networking/networking.cpp',
    'networking/remote_sink.cpp',
    'networking/remote_source.cpp',
    'solver/solver.cpp',
    'solver/tile.cpp',
    'solver/tile_header.cpp',
    'tilestream/cache.cpp',
    'tilestream/reliable_node.cpp',
    'tilestream/source_demux.cpp'
]

executable('fractal', sources: srcs, dependencies: deps, install: true)
