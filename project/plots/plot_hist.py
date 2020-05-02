import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt


def parse_line(line):
    s = []
    i = []
    r = []
    for processor in line.split("|")[0:-1]:
        data = processor.split(",")
        s.append(int(data[0]))
        i.append(int(data[1]))
        r.append(int(data[2]))
    return s, i, r


def read_file(filepath):
    S = []
    I = []
    R = []
    with open(filepath) as fp:
        line = fp.readline()
        while line:
            s, i, r = parse_line(line)
            S.append(s)
            I.append(i)
            R.append(r)
            line = fp.readline()
    return np.array(S), np.array(I), np.array(R)


if __name__ == "__main__":
    S, I, R = read_file(filepath="data/hist_data.txt")
    S_sum = np.sum(S, axis=1)
    I_sum = np.sum(I, axis=1)
    R_sum = np.sum(R, axis=1)
    plt.plot(np.array(list(range(S_sum.shape[0])))/10, S_sum, label="Susceptible")
    plt.plot(np.array(list(range(S_sum.shape[0])))/10, I_sum, label="Infectious")
    plt.plot(np.array(list(range(S_sum.shape[0])))/10, R_sum, label="Recovered")
    plt.legend()
    plt.xlabel("Time")
    plt.ylabel("People")
    plt.savefig("data/hist.png")
    plt.show()
