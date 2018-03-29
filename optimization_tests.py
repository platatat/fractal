from __future__ import print_function
import sys
import numpy as np
from PIL import Image
from matplotlib import pyplot
import time


LIM = 1000
RATIO = 1. / 8

WIDTH, HEIGHT = 128, 128


class Mandy:
    def __init__(self):
        self.hp_count = 0
        self.lp_count = 0


    def mand_solver(self, c, lim, z0=None):
        c_re, c_im = c
        if z0 is not None:
            z_re, z_im = z0
        else:
            z_re, z_im = c

        z_series = [(z_re, z_im)]

        for i in range(lim - 1):
            self.hp_count += 1
            z_re_new = z_re ** 2 - z_im ** 2 + c_re
            z_im_new = 2 * z_im * z_re + c_im
            z_re = z_re_new
            z_im = z_im_new

            z_series.append((z_re, z_im))

            if z_re ** 2 + z_im ** 2 > 4:
                return z_series

        return z_series


    def epsilon_solver(self, z_series, delta, error_lim):
        d_re, d_im = delta
        e_re, e_im = delta
        error_lim_sq = error_lim ** 2

        e_series = [(e_re, e_im)]

        for i, (z_re, z_im) in enumerate(z_series):
            self.lp_count += 1
            e_re_new = 2 * (e_re * z_re - e_im * z_im) + e_re ** 2 - e_im ** 2 + d_re
            e_im_new = 2 * (e_im * z_re + e_re * z_im) + 2 * e_re * e_im + d_im
            e_re = e_re_new
            e_im = e_im_new

            e_series.append((e_re, e_im))

            if e_re ** 2 + e_im ** 2 > error_lim_sq:
                return e_series, True

        return e_series, False


    def transform_x(self, px, center, radius):
        return (2.0 * px / float(WIDTH) - 1) * radius + center[0]


    def transform_y(self, py, center, radius):
        return (2.0 * py / float(HEIGHT) - 1) * radius + center[1]


    def draw(self, data, extent, fig_num, data2=None):
        data = data.T
        image_data = np.zeros((WIDTH, HEIGHT, 3), dtype=np.uint8)

        for x in range(WIDTH):
            for y in range(HEIGHT):
                value = data[x, y] * 255
                if value > 255:
                    raise Exception('Data value {} out of range.'.format(value))
                image_data[x, y] = np.array([value, value, value])
                if data2 is not None:
                    image_data[x, y, 2] = data2[x, y] * 255

        xmin = int(WIDTH * 0.5 - WIDTH * RATIO * 0.5)
        xmax = int(WIDTH * 0.5 + WIDTH * RATIO * 0.5)
        ymin = int(HEIGHT * 0.5 - HEIGHT * RATIO * 0.5)
        ymax = int(HEIGHT * 0.5 + HEIGHT * RATIO * 0.5)
        # image_data[xmin:xmax, ymin] = np.array([100, 100, 254])
        # image_data[xmin:xmax, ymax] = np.array([100, 100, 254])
        # image_data[xmin, ymin:ymax] = np.array([100, 100, 254])
        # image_data[xmax, ymin:ymax] = np.array([100, 100, 254])

        img = Image.fromarray(image_data)
        fig = pyplot.figure(fig_num, figsize=(4.5, 4.5))
        pyplot.imshow(img, extent=extent)
        fig.show()


    def slow_solver(self, center, radius):
        data = np.zeros((WIDTH, HEIGHT))

        cdf = np.zeros(LIM + 1)

        for x in range(WIDTH):
            x_s = self.transform_x(x, center, radius)
            print('{:.2f}'.format((x / float(WIDTH)) * 100), end='\r')
            sys.stdout.flush()
            for y in range(HEIGHT):
                y_s = self.transform_y(y, center, radius)
                z_series = self.mand_solver((x_s, y_s), LIM)
                soln = len(z_series)
                cdf[:soln] += 1. / (WIDTH * HEIGHT)
                data[x, y] = soln

        for x in range(WIDTH):
            for y in range(HEIGHT):
                soln = data[x, y]
                color = cdf[int(soln)]
                data[x, y] = color

        return data


    def fast_solver(self, center, radius, stride):
        cdf = np.zeros(LIM + 1)

        ref_data = []

        for x in range(int(WIDTH / stride)):
            ref_data.append([])
            x_s = self.transform_x((x + 0.5) * stride, center, radius)
            for y in range(int(HEIGHT / stride)):
                y_s = self.transform_y((y + 0.5) * stride, center, radius)
                z_series = self.mand_solver((x_s, y_s), LIM)
                soln = len(z_series)
                ref_data[x].append(z_series)

        data = np.zeros((WIDTH, HEIGHT))

        for x in range(WIDTH):
            x_s = self.transform_x(x, center, radius)
            print('{:.2f}'.format((x / float(WIDTH)) * 100), end='\r')
            sys.stdout.flush()
            for y in range(HEIGHT):
                ref_series = ref_data[int(x / stride)][int(y / stride)]
                y_s = self.transform_y(y, center, radius)
                delta = (x_s - ref_series[0][0], y_s - ref_series[0][1])
                e_series, diverged = self.epsilon_solver(ref_series, delta, radius * 2 ** 10)
                if diverged:
                    z_last = ref_series[len(e_series) - 1]
                    e_last = e_series[-1]
                    z_series = self.mand_solver((x_s, y_s), LIM - len(e_series), z0=(z_last[0] + e_last[0], z_last[1] + e_last[1]))
                    soln = len(e_series) + len(z_series)   
                else:
                    soln = len(ref_series)

                cdf[:soln] += 1. / (WIDTH * HEIGHT)
                data[x, y] = soln

        for x in range(WIDTH):
            for y in range(HEIGHT):
                soln = data[x, y]
                color = cdf[int(soln)]
                data[x, y] = color

        return data


    def reference_range_test(self, center, radius):
        data = np.zeros((WIDTH, HEIGHT))

        cdf = np.zeros(LIM + 1)

        z_ref_series = self.mand_solver(center, LIM)

        for x in range(WIDTH):
            x_s = self.transform_x(x, center, radius)
            print('{:.2f}'.format((x / float(WIDTH)) * 100), end='\r')
            sys.stdout.flush()
            for y in range(HEIGHT):
                y_s = self.transform_y(y, center, radius)
                delta = (x_s + center[1], y_s - center[0])
                z_series = self.mand_solver((y_s, x_s), LIM)
                e_series, _ = self.epsilon_solver(z_ref_series, delta, radius * 2 ** 10)
                soln = len(z_series)
                cdf[:soln] += 1. / (WIDTH * HEIGHT)
                data[x, y] = len(e_series) / float(len(z_series))

        return data


    def mand_render(self, center, radius, fig_num):
        data = self.fast_solver(center, radius, 8)
        # data = self.slow_solver(center, radius)

        extent = [
            center[0] - radius, 
            center[0] + radius, 
            center[1] - radius, 
            center[1] + radius
        ]

        self.draw(data, extent, fig_num)


    def multi_render(self, center, radius, n):
        for i in range(n):
            self.mand_render(center, radius * (RATIO ** i), i)

        pyplot.waitforbuttonpress()


    def speed_compare(self, center, radius):
        self.mand_render(center, radius, 0)
        print(self.hp_count, self.lp_count)
        pyplot.show()


    def test_epsilon(self):
        z0 = (-0.13874689 + 0.000018, -0.649553)
        delta = (0.00001, 0.00001)
        y0 = (z0[0] + delta[0], z0[1] + delta[1])
        z_series = self.mand_solver(z0, 1000)
        y_series = self.mand_solver(y0, 1000)

        e_actual = []

        for zn, yn in list(zip(z_series, y_series)):
            e_actual.append((yn[0] - zn[0], yn[1] - zn[1]))

        e_computed, _ = self.epsilon_solver(z_series, delta, delta[0] * 100000)

        # # pyplot.plot(e_computed)
        # pyplot.plot(z_series)
        # # pyplot.legend(['e_re', 'e_im', 'z_re', 'z_im'])
        # pyplot.show()

        for i, (zn, ea, ec) in enumerate(zip(z_series, e_actual, e_computed)):
            print('{:<6}:{:>16.8f}{:>16.8f}{:>16.8f}{:>16.8f}{:>16.8f}{:>16.8f}'.format(i, zn[0], zn[1], ea[0], ea[1], ec[0], ec[1]))



    def run(self):
        # pyplot.ion()
        # (-0.13874689, -0.649553)
        # self.multi_render((-0.5, 0), (1./8) ** 0, 1)
        # self.multi_render((-0.13874689 + 0.000018, -0.649553), (1./8) ** 5, 1)
        self.speed_compare((-0.13874689 + 0.000009, -0.649553), (1./8) ** 5)
        # self.test_epsilon()


if __name__ == '__main__':
    Mandy().run()