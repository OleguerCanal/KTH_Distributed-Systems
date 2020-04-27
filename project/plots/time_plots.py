import numpy as np
import matplotlib.pyplot as plt

class AutoVivification(dict):
    """Implementation of perl's autovivification feature."""
    def __getitem__(self, item):
        try:
            return dict.__getitem__(self, item)
        except KeyError:
            value = self[item] = type(self)()
            return value

def parse_line(line):
    data = line.split(",")
    return int(data[0]), int(data[1]), float(data[2]), float(data[3])

def read_file(filepath):
    data = AutoVivification()
    with open(filepath) as fp:
        line = fp.readline()
        while line:
            p, n, ws, t = parse_line(line)
            data[ws][p][n] = t
            line = fp.readline()
    return data

data = read_file("TIMES.txt")

fig, axs = plt.subplots(len(data.keys()))
for ws_indx, ws in enumerate(data.keys()):
    for p in data[ws].keys():
        people = []
        times = []
        for n in data[ws][p].keys():
            people.append(n)
            times.append(data[ws][p][n])
        axs[ws_indx].plot(people, times, label=str(p) + " processors")
        axs[ws_indx].set_ylabel("World size: " + str(ws) + "\nTime [s]")

axs[-1].set_xlabel("People Simulated")
plt.legend()
plt.suptitle("Execution Times")
plt.show()