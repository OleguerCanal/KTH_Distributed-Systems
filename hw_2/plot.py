import numpy as np
from matplotlib import pyplot as plt

def f(x):
    return x*(x-1)

with open('poisson.txt', 'r') as file:
    data = file.read().replace('\n', '')

data = data.split(" ")
data = [float(i) for i in data]

a = np.linspace(0, 1, 1000)
a = f(a)

plt.plot(data)
plt.plot(a)
plt.show()