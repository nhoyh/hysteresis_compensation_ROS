import numpy as np
import bdh_boucwen
import time
import pyswarms as ps

def read_data():
    read_file = np.genfromtxt('./selected/0.txt', delimiter=',')

    global tt, des_pos, des_vel, act_pos, dt
    tt = []
    des_pos = []
    des_vel = []
    act_pos = []

    for i in range(2000):  ## DATA={t(i), d_pos(i), d_vel(i), a_pos(i)},i=1..2000
        if i > 0:
            if read_file[i - 1, 0] != read_file[i, 0]:
                tt.append(read_file[i, 0])
                des_pos.append(read_file[i, 1])
                des_vel.append(read_file[i, 2])
                act_pos.append(read_file[i, 3])

    # print(len(tt), len(des_pos), len(des_vel), len(act_pos))
    dt = 0.001

init_range_low = [-1, 0, -0.1, -5, 0, 0, -5]
init_range_high = [0.1, 0.1, 0.1, 10, 5, 25, 5]
bounds = (init_range_low, init_range_high)


# read data
read_data()

# def fitness_func(X):
#     # print("x",X)
#     fitness = 1.0/ (bdh_boucwen.CostFunction(X, des_vel, dt, des_pos, act_pos)+0.000001)
#     return fitness

def fitness_func(X):
    print("x",len(X))
    for i in range(len(X)):
        fitness = bdh_boucwen.CostFunction(X[i], des_vel, dt, des_pos, act_pos)
        print("fitness",fitness)
    return fitness


def fitness_func_sum(X):
    pop_fitness = []
    # Calculating the fitness value of each solution in the current population.
    for sol_idx, sol in enumerate(X):
        fitness = 1.0/ (bdh_boucwen.CostFunction(sol, des_vel, dt, des_pos, act_pos)+0.000001)
        pop_fitness.append(fitness)

    best_match_idx =np.where(pop_fitness == np.max(pop_fitness))[0][0]
    best_solution = X[best_match_idx, :]
    final_cost = bdh_boucwen.CostFunction(best_solution, des_vel, dt, des_pos, act_pos)
    return final_cost

def PSO_run():

    # Set-up optimizer
    options = {'c1': 0.5, 'c2': 0.2, 'w': 0.9}
    optimizer = ps.single.GlobalBestPSO(n_particles=50, dimensions=7, options=options)
    global cost, pos
    cost, pos = optimizer.optimize(fitness_func_sum, iters=100)



def main():
    print("Usage: python pso.py")

    t_start = time.time()
    PSO_run()
    t_finish = time.time()

    out_cost = bdh_boucwen.CostFunction(pos, des_vel, dt, des_pos, act_pos)
    print("processing time=", (t_finish - t_start))
    print("final cost=", out_cost, "final pos =", pos)

if __name__ == "__main__":
    main()
