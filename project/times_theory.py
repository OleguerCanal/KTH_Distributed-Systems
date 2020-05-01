import numpy as np
import math

def get_m(n, px, py, s):
    def get_jump_prob_side(d):
        k1 = s/(d*np.sqrt(2*np.pi))
        k2 = -(d**2)/(2*(s**2))
        return k1*(1-np.exp(k2))
    m = 2*(get_jump_prob_side(px) + get_jump_prob_side(py))*n
    return m

def get_k(n, px):
    k = n*INFECTION_DISTANCE/px
    return k

def get_i(n, px, py):
    i = 2*(INFECTION_DISTANCE/px + INFECTION_DISTANCE/py)*n
    return i

if __name__ == "__main__":
    # Inputs
    t = 15000
    P = 4
    p_x = 2
    WS = 500.0
    SPEED = 1.0
    INFECTION_DISTANCE = 1.0
    N = 1000

    # Computed
    p_y = float(P/p_x)
    px = float(WS/p_x)  # perimeter in x
    py = float(WS/p_y)  # perimeter in x
    s = 1 # SIGMA

    for N in [1000, 2000, 4000, 8000, 16000, 32000]:
        T_1 = N + get_m(N, WS, WS, s) + N*math.log(N, 2) + get_k(N, WS)*N
        n = float(N/P)
        m = get_m(n, px, py, s)
        k = get_k(n, px)
        i = get_i(n, px, py)
        print("T_1:", T_1)
        print("m:", m)
        print("k:", k)
        print("i:", i)
        T_p = n + m + n*math.log(n, 2) + k*n + (8000 + 2400*(m + i))
        print("Speedup of", N, ":", T_1/T_p)


