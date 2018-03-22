
import scipy.misc
import numpy as np

# Image size
width = 600
height = 400
channels = 3

# Create an empty image
img = np.zeros((height, width, channels), dtype=np.uint8)

with open('solve_dat.txt', 'r') as f:
    f.readline()
    for l in f:
        line = l[:-1].split()
        if (len(line) < 3):
            break
        x = int(float(line[0]) / (2 ** 20) * 200) + 400
        y = int(float(line[1]) / (2 ** 20) * 200) + 200
        v = int(line[2])
        if (v == -1):
            img[y][x][0] = 200
            img[y][x][1] = 200
            img[y][x][1] = 200

# Display the image
scipy.misc.imshow(img)
