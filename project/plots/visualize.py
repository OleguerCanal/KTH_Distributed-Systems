import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from itertools import cycle
import time
import glob
import sys


class AnimatedScatter(object):
    """An animated scatter plot using matplotlib.animations.FuncAnimation."""
    def __init__(self, numpoints=50):
        self.read_file()
        self.i = -1
        self.stream = self.data_stream()

        # Setup the figure and axes...
        self.fig, self.ax = plt.subplots()
        # Then setup FuncAnimation.
        self.ani = animation.FuncAnimation(self.fig, self.update, frames=self.length, interval=100, 
                                          init_func=self.setup_plot, blit=False)
        
        Writer = animation.writers['ffmpeg']
        writer = Writer(fps=30, metadata=dict(artist='Me'), bitrate=1800)
        self.ani.save('data/evolution.mp4', writer=writer)
        print("Saved")
        # self.ani.save('animation.gif', writer='imagemagick', fps=10)
        # self.ani.save('animation.gif', writer='imagemagick', fps=10)

    def setup_plot(self):
        """Initial drawing of the scatter plot."""
        x, y, s, c = next(self.stream).T
        self.scat = self.ax.scatter(x, y, c=c, s=s, vmin=0, vmax=1,
                                    cmap="RdYlGn")
        self.ax.axis([0, 20, 0, 20])
        self.ax.set_aspect('equal')
        # For FuncAnimation's sake, we need to return the artist we'll be using
        # Note that it expects a sequence of artists, thus the trailing comma.
        return self.scat,

    def data_stream(self):
        """Generate a random walk (brownian motion). Data is scaled to produce
        a soft "flickering" effect."""
        # xy = (np.random.random((self.numpoints, 2))-0.5)*10
        # s, c = np.random.random((self.numpoints, 2)).T
        # while True:
        #     xy += 0.03 * (np.random.random((self.numpoints, 2)) - 0.5)
        #     s += 0.05 * (np.random.random(self.numpoints) - 0.5)
        #     yield np.c_[xy[:,0], xy[:,1], s, c]
        while True:
            self.i += 1
            data = self.data[self.i%self.length]
            c = 10.0 * (np.ones(len(data[0])))
            yield np.c_[np.array(data[0]).T, np.array(data[1]).T, c, np.array(data[2]).T]

    def update(self, i):
        """Update the scatter plot."""
        data = next(self.stream)

        # Set x and y data...
        self.scat.set_offsets(data[:, :2])
        # Set sizes...
        self.scat.set_sizes(50*np.ones(data[:, 2].shape))
        # Set colors..
        # print(data[:, 3])
        self.scat.set_array(data[:, 3])

        # We need to return the updated artist for FuncAnimation to draw..
        # Note that it expects a sequence of artists, thus the trailing comma.
        return self.scat,

    def __parse_line(self, line):
        x = []
        y = []
        status = []
        # for processor in line.split("|")[0:-1]:
        processor = line.replace("|", "")
        for person in processor.split(";")[0:-1]:
            person_info = person.split(",")
            px = float(person_info[0])
            py = float(person_info[1])
            ps = int(person_info[2])
            # if px > 1:
            #     print(px)
            x.append(px)
            y.append(py)
            status.append(ps)
        return x, y, status

    def read_file(self, filepath = "data/evolution.txt"):
        self.data = []
        with open(filepath) as fp:
                line = fp.readline()
                while line:
                    x, y, status = self.__parse_line(line)
                    # color = ['green' if l == 0 else 'red' for l in status]
                    self.data.append([x, y, 1-np.array(status)/2.0])
                    line = fp.readline()
        self.length = len(self.data)
        # self.data = cycle(self.data)


if __name__ == '__main__':
    a = AnimatedScatter()
    plt.show()