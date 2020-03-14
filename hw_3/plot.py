import numpy as np
from matplotlib import pyplot as plt
import matplotlib.image as mpimg
import time

def parse_line(line):
    row = []
    for elem in line.split(" ")[0:-1]:
        row.append(int(elem))
    return 

if __name__ == "__main__":
    filepaths = ["1", "2", "4", "6", "8"]

    for filepath in filepaths:
        n_s = []
        times = []
        with open(filepath) as fp:
            line = fp.readline()
            while line:
                n_s.append(int(line.split(" ")[0]))
                times.append(float(line.split(" ")[1]))
                line = fp.readline()
        print(times)
        plt.plot(n_s, times, label="Processors: " + filepath)

    plt.legend()
    plt.show()