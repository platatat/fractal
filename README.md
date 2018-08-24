# Fractal

Mandelbrot solver and renderer.

## Build Instructions

For the initial setup, install `meson` and make a build directory. You only need to run `meson build` once.

```bash
apt install meson
mkdir build
meson build
```

Run ninja from inside the build directory to build. One process runs a server and another runs a client. The server must be started first.

```bash
cd build
ninja
./fractal server PORT
./fractal client IP:PORT
```

## To-Do List

- Epsilon solver
- Dynamic iteration updates
- Store tiles to disk
- Dynamic coloring
- Faster client-side rendering
- Smooth coloring
- Multi-threaded CPU solver
- Non-local networking