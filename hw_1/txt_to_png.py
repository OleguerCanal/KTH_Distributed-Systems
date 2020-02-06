import numpy as np
from matplotlib import pyplot as plt
import matplotlib.image as mpimg

def parse_line(line):
    row = []
    for elem in line.split(" ")[0:-1]:
        row.append(int(elem))
    return row

if __name__ == "__main__":
    filepath = "image.txt"

    img = []
    with open(filepath) as fp:
        line = fp.readline()
        while line:
            img_row = parse_line(line)
            img.append(img_row)
            line = fp.readline()
            
    img = np.array(img)
    imgplot = plt.imshow(img)
    plt.show()