
import scipy.misc
import numpy as np

# Image size
width = 640
height = 480
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

        s_id = int(line[0])
        s_addr = int(line[1])
        if (line[2] == 'x'):
            continue
        s_val = int(line[2])

        x = s_addr % width
        y = s_addr / width
        if s_val > vmax:
            vmax = s_val
        if (s_val == -1):
            img[y][x][0] = 0
            img[y][x][1] = 0
            img[y][x][2] = 0
        else:
            img[y][x][0] = 255 - (s_val * 42)
            img[y][x][1] = 255 - (s_val * 42)
            img[y][x][2] = 255 - (s_val * 42)
    print vmax

# Display the image
scipy.misc.imshow(img)
