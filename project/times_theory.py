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
    # t = 15000
    P = 32
    p_x = 4
    WS_base = 500.0
    SPEED = 1.0
    INFECTION_DISTANCE = 1.0
    # N = 1000

    # Computed
    # alpha = 755.7557557557558
    # beta = -3588
    alpha = 100
    beta = 10000


    min_error = 50
    min_params = None
    # for alpha in np.linspace(0, 5000, 1000):
    #     for beta in np.linspace(-5000, 5000, 1000):

    sols = []
    for en, num in enumerate([1e6]):
        N = num*num*1000
        WS = WS_base*num
        p_y = float(P/p_x)
        px = float(WS/p_x)  # perimeter in x
        py = float(WS/p_y)  # perimeter in x
        
        s = 1 # SIGMA
        T_1 = N + get_m(N, WS, WS, s) + N*math.log(N, 2) + get_k(N, WS)*N
        n = float(N/P)
        m = get_m(n, px, py, s)
        k = get_k(n, px)
        i = get_i(n, px, py)
        print("T_1:", T_1)
        print("m:", m)
        print("k:", k)
        print("i:", i)
        T_p = n + m + n*math.log(n, 2) + k*n + alpha*(m + i) + beta  # 8000 because t_startup is 1000 times slower than t_a or t_data, 2400 should be 24 but had to tune it a bit
        sols.append(T_1/T_p)
        print("Speedup of", N, ":", T_1/T_p)

    error = np.linalg.norm(np.array(sols) - np.array([15, 40, 50]))
    if error < min_error:
        min_error = error
        min_params = [alpha, beta]
            # print("error:", error)
    print(min_error)
    print(min_params)