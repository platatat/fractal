#!/bin/bash

set -e

mkdir -p build
iverilog -g2005-sv -o build/a.out $1
vvp build/a.out
