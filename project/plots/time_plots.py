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
            p, px, n, t = parse_line(line)
            data[p][px][n] = t  # No need to divide since the result is relative, just make sure its consistent
            line = fp.readline()
    return data

data = read_file("TIMES.txt")

fig, axs = plt.subplots(1, len(data.keys())-1)
for p_indx, p in enumerate(data.keys()):
    if p == 1:
        continue
    for p_x in data[p].keys():
        people = []
        times = []
        for n in data[p][p_x].keys():
            people.append(n/1000)
            times.append(data[1][1][n]/data[p][p_x][n])
        axs[p_indx-1].plot(people, times, label=str(p_x) + "x" + str(int(p/p_x)) + " processors")
        axs[p_indx-1].set_xlabel("x1000 People")
        axs[p_indx-1].set_ylim(1, 8)
        axs[p_indx-1].legend()

axs[0].set_ylabel("Speedup")
plt.suptitle("Execution Times")
plt.savefig("figures/times.png")
plt.show()