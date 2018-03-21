#!/bin/bash

set -e

mkdir -p build
iverilog -o build/a.out $1
vvp build/a.out
