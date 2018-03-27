
import scipy.misc
import numpy as np

# Image size
width = 99
height = 66
channels = 3

# Create an empty image
img = np.zeros((height, width, channels), dtype=np.uint8)

for x in range(width):
    for y in range(height):
        img[y][x][0] = 200
        img[y][x][1] = 0
        img[y][x][2] = 0


with open('solve_dat.txt', 'r') as f:
    f.readline()
    vmax = 0
    for l in f:
        line = l[:-1].split()
        if (len(line) < 3):
            break
        x = int(float(line[0]) / (2 ** 20) * 33) + 66
        y = int(float(line[1]) / (2 ** 20) * 33) + 33
        v = int(line[2])
        if v > vmax:
            vmax = v
        if (v == -1):
            img[y][x][0] = 0#200
            img[y][x][1] = 0#200
            img[y][x][2] = 0#200
        else:
            img[y][x][0] = 255 - (v * 36)
            img[y][x][1] = 255 - (v * 36)
            img[y][x][2] = 255 - (v * 36)
    print vmax

# Display the image
scipy.misc.imshow(img)
