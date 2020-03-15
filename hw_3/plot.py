import numpy as np
from matplotlib import pyplot as plt
import matplotlib.image as mpimg
import time
import glob

def parse_line(line):
    row = []
    for elem in line.split(" ")[0:-1]:
        row.append(int(elem))
    return 

if __name__ == "__main__":
    # filepaths = ["1000000", "10000000", "100000000", "1000000000"]
    # filepaths = ["100000000", "1000000000"]
    # filepaths = ["1000000"]

    filepaths = list(glob.glob('10*'))
    print(filepaths)

    for filepath in filepaths:
        processors = []
        times = []
        with open(filepath) as fp:
            line = fp.readline()
            while line:
                processors.append(int(line.split(" ")[0]))
                times.append(float(line.split(" ")[1]))
                line = fp.readline()
        # ASSUMING TIMES[0] IS 1 PROCESSOR!!!
        times = [times[0] / time  for time in times] # Compute speedup
        plt.plot(processors, times, label="Data size: " + str("{:.1e}".format(int(filepath))))

    plt.legend()
    plt.show()