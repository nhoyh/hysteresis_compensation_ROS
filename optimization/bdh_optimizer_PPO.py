import numpy as np
import time
import pyswarms as ps
import os, csv
import argparse
import math
import matplotlib.pyplot as plt
from pyswarms.utils.plotters import plot_cost_history

def Boucwen(X, h, des_vel):  # X = {x[0]..x[6]} x[0]=A, x[1]=B, x[2]=C, x[3]=n, x[4] = alpha, x[5] = beta, x[6] = gamma

    #if h == 0:
    #    h = 0.01
    h_dot = X[0] * des_vel - X[1] * abs(des_vel) * pow(abs(h), X[3] - 1) * h - X[2] * des_vel * pow(abs(h), X[3])
    return h_dot


def RK4_Boucwen(X, h, des_vel, dt):  # Solving ODE
    # k1
    h_k0 = h
    h_dot_k1 = Boucwen(X, h, des_vel)
    phi_k1 = h_dot_k1
    #print("phi_k1",phi_k1)
    h_k1 = h_k0 + 0.5 * dt * h_dot_k1
    # print("h_k1",h_k1)

    # k2
    h_dot_k2 = Boucwen(X, h_k1, des_vel)
    phi_k2 = phi_k1 + 2 * h_dot_k2
    h_k2 = h_k0 + 0.5 * dt * h_dot_k2
    #print("h_k2",h_k2)

    # k3
    h_dot_k3 = Boucwen(X, h_k2, des_vel)
    phi_k3 = phi_k2 + 2 * h_dot_k3
    h_k3 = h_k0 + dt * h_dot_k3
    #print("h_k3",h_k3)

    # 4
    h_dot_k4 = Boucwen(X, h_k3, des_vel)

    # RK4
    H = h_k0 + (phi_k3 + h_dot_k4) * dt / 6  # h_k0 + (h_dot_k1 + 2*h_dot_k2 + 2*h_dot_k3 + h_dot_k4)*dt/6
    #print("H",H)

    return H


def CostFunction(X, des_vel, dt, des_pos, act_pos):

  H = np.zeros(len(des_pos))
  X = np.array(X)
  des_vel = np.array(des_vel)
  des_pos = np.array(des_pos)
  act_pos = np.array(act_pos)

  H[0] = X[0]*des_vel[0]

  for i in range(1, len(des_pos)):#(len(des_pos)-1):
     H[i] = RK4_Boucwen(X, H[i-1], des_vel[i],dt)

  #print(H)
  Cost_J = 0

  for i in range(len(des_pos)):
      # Cost_J += pow((X[4]*des_pos[i] -X[5]*H[i] +X[6] - act_pos[i]), 2)
      Cost_J += pow((X[4] * des_pos[i] - X[5] * H[i] - act_pos[i]), 2)
  Cost_J = Cost_J/len(des_pos)

  #print("len(des_pos), cost", len(des_pos), Cost_J)
  if math.isnan(Cost_J):
      Cost_J = 999999
  #else:
  #    Cost_J = Cost_J
  #print("J = ",Cost_J)
  return Cost_J

def save_results_csv(fname, results, header=0):
    """Saves results in csv file
    Args:
        fname (str): name of the file
        results (list): list of prec, rec, F1, rds
    """
    new_rows = []
    ss = os.path.isfile(fname)
    if not os.path.isfile(fname):
        args = fname.split("/")[:-1]
        directory = os.path.join(*args)
        if not os.path.exists(directory):
            os.makedirs(directory)

        with open(fname, "wt") as f:
            writer = csv.writer(f)
            if header == 0:
                writer.writerows(
                    [
                        [
                            "Optimized cost",
                            "value",
                            "time",
                            "data type",
                        ]
                    ]
                )

    with open(fname, "at") as f:
        # Overwrite the old file with the modified rows
        writer = csv.writer(f)
        new_rows.append(results)  # add the modified rows
        writer.writerows(new_rows)


def read_data():
    global data_num
    data_num = parser.parse_args().data
    target_data = './selected/' + data_num + '.txt'
    read_file = np.genfromtxt(target_data, delimiter=',')

    global tt, des_pos, des_vel, act_pos, dt
    tt = []
    des_pos = []
    des_vel = []
    act_pos = []

    data_size = np.size((read_file[:, 0]))

    for i in range(data_size):  ## DATA={t(i), d_pos(i), d_vel(i), a_pos(i)},i=1..2000
        if i > 0:
            if read_file[i - 1, 0] != read_file[i, 0]:
                tt.append(read_file[i, 0])
                des_pos.append(read_file[i, 1])
                des_vel.append(read_file[i, 2])
                act_pos.append(read_file[i, 3])

    # print(len(tt), len(des_pos), len(des_vel), len(act_pos))
    dt = 0.001

def fitness_func(X):
    print("x",len(X))
    for i in range(len(X)):
        fitness = CostFunction(X[i], des_vel, dt, des_pos, act_pos)
        print("fitness",fitness)
    return fitness


def fitness_func_sum(X):
    pop_fitness = []
    # Calculating the fitness value of each solution in the current population.
    for sol_idx, sol in enumerate(X):
        fitness = 1.0/ (CostFunction(sol, des_vel, dt, des_pos, act_pos)+0.000001)
        pop_fitness.append(fitness)

    best_match_idx =np.where(pop_fitness == np.max(pop_fitness))[0][0]
    best_solution = X[best_match_idx, :]
    final_cost = CostFunction(best_solution, des_vel, dt, des_pos, act_pos)
    return final_cost

def PSO_run():

    # Set-up optimizer
    options = {'c1': 0.5, 'c2': 0.2, 'w': 0.9}
    optimizer = ps.single.GlobalBestPSO(n_particles=50, dimensions=7, options=options,bounds=bounds)
    global cost, pos
    best_cost, best_pos = optimizer.optimize(fitness_func_sum, iters=100)
    # Obtain the cost history

    return best_cost, best_pos

def main():
    global parser
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('--data', dest='data', default='0', help='data #. usage: python bdh_optimizer_PPO.py --data=30')
    args = parser.parse_args()

    # read data
    read_data()
    init_range_low = [5.00000e-02, 1.04000e-02, 4.30000e-03, 5.36670e+00, 9.46500e-01, 1.76325e+01, -1.51070e+00]
    init_range_high = [2.10000e-02, 6.80000e-03, 6.30000e-03, 2.21470e+01, 6.86300e-01, 3.25269e+01, 5.12300e-01]
    # init_range_low = [0.0282,0.0013,0.0007,5.1632,0.7196,14.6929,-2.7508] # 0.25 0.75
    # init_range_high = [0.052,0.011,0.0091,11.4216,0.8844,24.4927,-0.7128]
    low_bound = []
    upper_bound = []
    for i in range(7):
        low_bound.append(min(init_range_low[i], init_range_high[i]))
        upper_bound.append(max(init_range_low[i], init_range_high[i]))

    global bounds
    bounds = (low_bound, upper_bound)

    t_start = time.time()
    best_cost, best_pos = PSO_run()
    t_finish = time.time()

    print("processing time=", (t_finish - t_start))
    print("final cost=", best_cost, "final pos =", best_pos)

    results = [best_cost, best_pos, t_finish - t_start, args.data]
    location = "result/"
    save_results_csv(location + "/pso_result_01.csv".format(""), results, header=0)

    # Plot the cost
    #plot_cost_history(optimizer.cost_history)
    #plt.show()

if __name__ == "__main__":
    main()
