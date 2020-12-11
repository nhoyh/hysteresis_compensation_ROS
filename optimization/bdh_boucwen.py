#!/usr/bin/env python
import numpy as np
import math
from numpy import genfromtxt
import os
import pandas as pd

def init_Random_Population(population_size, chromosome_size):
    #Creating the initial population.
    #based on human knowledgement (Heuristic)
    new_population = []
    X=[]
    low_bound = [-0.01, 0, -0.01, -1, 0, 0, -5]
    upper_bound = [0.05, 0.05, 0.01, 10, 5, 25, 5]
    #low_bound = [-1, -1, -1, -10, -1, -1, -5]
    #upper_bound = [1, 1, 1, 10, 5, 25, 5]
    # Defining the population size.
    for i in range(population_size):
        for j in range(chromosome_size):
            X.append(round(np.random.uniform(low_bound[j], upper_bound[j]),4))
            #print(X)
        new_population.append(X)
        X = []

    return new_population

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


  #list to array
  H = np.zeros(len(des_pos))
  X = np.array(X)
  des_vel = np.array(des_vel)
  des_pos = np.array(des_pos)
  act_pos = np.array(act_pos)

  H[0] = X[0]*des_vel[0]

  for i in range(1, len(des_pos)):#(len(des_pos)-1):
     H[i] = RK4_Boucwen(X, H[i-1], des_vel[i],dt)

  #print(H)
  global Cost_J
  Cost_J = 0

  for i in range(len(des_pos)):
      Cost_J += pow((X[4]*des_pos[i] -X[5]*H[i] +X[6] - act_pos[i]), 2)

  Cost_J = Cost_J/len(des_pos)

  #print("len(des_pos), cost", len(des_pos), Cost_J)
  if math.isnan(Cost_J):
      Cost_J = 999999
  #else:
  #    Cost_J = Cost_J
  #print("J = ",Cost_J)
  return Cost_J


'''
def CostFunction_GA_SUM(X, des_vel, dt, des_pos, act_pos):
    # list to array
    H = np.zeros(len(des_pos))
    X = np.array(X)
    des_vel = np.array(des_vel)
    des_pos = np.array(des_pos)
    act_pos = np.array(act_pos)

    # print(des_vel)
    global Sum_Cost
    Sum_Cost = 0
    for j in range(len(X)):

        H[0] = X[j][0] * des_vel[0]
        for i in range(len(des_pos) - 1):  # (len(des_pos)-1):
            H[i + 1] = RK4_Boucwen(X[j], H[i], des_vel[i + 1], dt)

        global Cost_J
        Cost_J = 0

        for i in range(len(des_pos)):
            Cost_J += pow((X[j][4] * des_pos[i] - X[j][5] * H[i] + X[j][6] - act_pos[i]), 2)

        Cost_J = Cost_J / len(des_pos)
        if math.isnan(Cost_J):
            Cost_J = 0
        Sum_Cost += Cost_J
    Sum_Cost = Sum_Cost / len(X)
    print("J = ", Sum_Cost)
    return Sum_Cost
'''

def read_data():
    read_file = genfromtxt('./selected/0.txt', delimiter=',')

    global tt, des_pos, des_vel, act_pos
    tt = []
    des_pos = []
    des_vel = []
    act_pos = []

    for i in range(2000):
        if i > 0:
            if read_file[i - 1, 0] != read_file[i, 0]:  # remove redundant data
                tt.append(read_file[i, 0])
                des_pos.append(read_file[i, 1])
                des_vel.append(read_file[i, 2])
                act_pos.append(read_file[i, 3])

    # print(len(tt), len(des_pos), len(des_vel), len(act_pos))
    dt = 0.001

    # initialization
    X = [0.045, 0.016, 0.007, 5.25, 0.95, 19.41, -1.51]
    Cost_MSE = CostFunction(X, des_vel, dt, des_pos, act_pos)
    print(Cost_MSE)

if __name__ == "__main__":
    read_data()
