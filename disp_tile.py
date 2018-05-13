#!/usr/bin/env python3
import numpy as np
from matplotlib import pyplot

height = 64
width = 64
img = np.zeros(width * height, dtype=np.uint16)

with open("tile.txt", 'r') as f:
    for line in f:
        if line[0] != "!":
            continue

        _, addr, data = line.split(",")
        addr = int(addr)
        data = int(data)

        if data == 65535:
            data = 17

        img[addr // 2] = data

img = img.reshape((width, height))

pyplot.imshow(img)
pyplot.show()
