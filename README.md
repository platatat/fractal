# Fractal

Mandelbrot solver and renderer.

## To Build

First time:

```bash
apt install meson
mkdir build
meson build
```

Every time:

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