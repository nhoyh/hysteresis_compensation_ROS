import pygad
import rospy
import numpy as np
import time
from scipy.optimize import minimize
import math
import argparse
import os
import csv
from da_msg.msg import command


optimized_result = '/home/ldg/catkin_ws/src/optimizer_input/optimized_result.txt'

pub_optimized_param = rospy.Publisher('optimization_topic',command,queue_size=1)

def read_data():
#    data_num = parser.parse_args().data
#    target_data = './selected/' + data_num +'.txt'
    #target_data = '/home/ldg/catkin_ws/src/optimizer_input/output.txt'
    target_data = '/home/ldg/Desktop/nobaek/result_data/conf1_2/60sin2/none5.txt'
    read_file = np.genfromtxt(target_data, delimiter=',')

    global tt, des_pos, des_vel, act_pos, dt
    tt = []
    des_pos = []
    des_vel = []
    act_pos = []

    for i in range(4000):  ## DATA={t(i), d_pos(i), d_vel(i), a_pos(i)},i=1..2000
        if i > 0:
            if read_file[i - 1, 0] != read_file[i, 0]:
                tt.append(read_file[i, 0]) # time
                des_pos.append(read_file[i, 1]) # desired pos
                des_vel.append(read_file[i, 2]) #desired vel
                act_pos.append(read_file[i, 4]) #actual pos


    dt = 0.001

def init_Random_Population(population_size, chromosome_size):
    #Creating the initial population.
    #based on human knowledgement (Heuristic)
    new_population = []
    X=[]
#    low_bound = [0.0195,0.0002,0.0001,3.4378,0.6863,12.9245,-3.28] # 0 1
#    upper_bound = [0.0682,0.0127,0.0099,16.3298,0.9465,37.0849,0.8304]
    #low_bound = [0.0282,0.0013,0.0007,5.1632,0.7196,14.6929,-2.7508] # 0.25 0.75
    #conf0 param = 0.242, 0.8042, -0.710, 8.571, 0.455, 7.887,-2.16704 [-0.1, 0, -0.1, 0, 0, 0, -10] ~ [0.5, 0.4, 0.01, 20, 5, 10, 10]
    low_bound = [-0.1, -0.1, 0, 0, 0, 0]
    upper_bound = [1, 0.4, 0.1, 20, 5, 40]
    # Defining the population size.
    for i in range(population_size):
        for j in range(chromosome_size):
            X.append(round(np.random.uniform(low_bound[j], upper_bound[j]),4))
        new_population.append(X)
        X = []

    return new_population, low_bound, upper_bound

def Boucwen(X, h, des_vel):  # X = {x[0]..x[6]} x[0]=A, x[1]=B, x[2]=C, x[3]=n, x[4] = alpha, x[5] = beta, x[6] = gamma

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


def CostFunction(X):
  global tt, des_pos, des_vel, act_pos, dt
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
  Cost_J = 0

  for i in range(len(des_pos)):
      Cost_J += pow((X[4]*des_pos[i] -X[5]*H[i]- act_pos[i]), 2)

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
                            "Initial cost",
                            "Optimized cost",
                            "Value",
                            "# of iter",
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


def run(): #def run(msg):
    #global parser
    #parser = argparse.ArgumentParser(description='')
    #parser.add_argument('--method', dest='method', default='Powell', help='method name. usage: python scipy_optimizer.py --method=Powell')
    #parser.add_argument('--data', dest='data', default='30', help='data #. usage: python scipy_optimizer.py --data=30')
    #args = parser.parse_args()
    #method = 
    print("start optimization")  #method: Nelder-Mead, Powell, CG, BFGS, Newton-CG, L-BFGS-B, TNC, COBYLA,  SLSQP, dogleg, trust-ncg, trust-exact, trust-krylov
    
    read_data()
    init_point, low_bound, upper_bound = init_Random_Population(1,6)
    init_point = np.array(init_point[0])
    bnds = [low_bound,upper_bound]


    initial_cost = CostFunction(init_point)

    t_start = time.time()
    cnt = 0
    while (1):
        optimum = minimize(CostFunction,init_point,method="Nelder-Mead",options={'xtol':1e-8,'disp':True,'maxiter':1000}) #Nelder-Mead
#        optimum = minimize(CostFunction,init_point,method=args.method,bounds=bnds,options={'xtol':1e-8,'disp':True,'maxiter':1000})
        cnt = cnt + 1
        #print("Trial #: ",cnt, args.data, args.method)
        if CostFunction(optimum.x) < 2:
            break
        if time.time() - t_start > 20:
            print("end time")
            break
    t_end = time.time()
    print('########### RESULT ############ (# of iter, init cost, fin cost, X, time)')
#    print(optimum)
    #if args.method == 'COBYLA':
    #    nit = optimum.nfev
    #else:
    #    nit = optimum.nit
    print(initial_cost)
    print(CostFunction(optimum.x))
    print(np.round(optimum.x,4))
    print(t_end-t_start)

    #results = [initial_cost, CostFunction(optimum.x),np.round(optimum.x,4),nit,t_end-t_start,args.data]
    #location = "result/{}/".format(args.method)
    #save_results_csv(location+"/results{}.csv".format(""), results, header=0)

    #pub
    #print("pub msg",np.round(optimum.x[0],4),np.round(optimum.x[1],4),np.round(optimum.x[2],4),np.round(optimum.x[3],4),np.round(optimum.x[4],4),np.round(optimum.x[5],4),np.round(optimum.x[6],4) )
    #pub_msg = command()
    #pub_msg.parameter_value = [np.round(optimum.x[0],4),np.round(optimum.x[1],4),np.round(optimum.x[2],4),np.round(optimum.x[3],4),np.round(optimum.x[4],4),np.round(optimum.x[5],4),np.round(optimum.x[6],4)]
    #pub_optimized_param.publish(pub_msg)

    #write result
    global text_file
    text_file = open(optimized_result, "w")
    text_file.write(str(np.round(optimum.x[0],4)))
    text_file.write(",")
    text_file.write(str(np.round(optimum.x[1],4)))
    text_file.write(",")
    text_file.write(str(np.round(optimum.x[2],4)))
    text_file.write(",")
    text_file.write(str(np.round(optimum.x[3],4)))
    text_file.write(",")
    text_file.write(str(np.round(optimum.x[4],4)))
    text_file.write(",")
    text_file.write(str(np.round(optimum.x[5],4)))
    #text_file.write(",")
    #text_file.write(str(np.round(optimum.x[6],4)))
    text_file.close()


def main():
    rospy.init_node('boucwen_optimization')
    optimization_topic = "optimization_start"
    print("nb optimization start!")

    #rospy.Subscriber(optimization_topic, command, run)
    #rospy.spin()
    run()

if __name__ == "__main__":
    main()