# Fractal

Mandelbrot solver and renderer.

## Build Instructions

For the initial setup, install `meson` and make a build directory. You only need to run `meson build` once.

```bash
apt install meson
mkdir build
meson build
```

Run ninja from inside the build directory to build the project. Run a server in one process and a client in another. The server must be started first.

```bash
cd build
ninja
./fractal server PORT
./fractal client IP:PORT
```

## To-Do List

- More than 2 server connections
- Epsilon solver
- Dynamic iteration updates
- Store tiles to disk
- Dynamic coloring
- Faster client-side rendering
- Smooth coloring
- Multi-threaded CPU solver