#!/bin/bash
cd /home/ubuntu
git clone https://github.com/platatat/fractal.git
apt update -y
apt install -y python3-pip g++ libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libgmp-dev libcairo2-dev ninja-build
pip3 install meson
cd fractal
mkdir build
meson build
cd build
ninja
./fractal worker 54.90.220.130:7000 10 1
