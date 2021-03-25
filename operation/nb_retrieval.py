#!/usr/bin/env python
import sys
import numpy as np
import os
import tensorflow as tf
import nb_network_test
from scipy.spatial import distance
import cv2
import operator
import time
from numpy import linalg as LA
from sklearn.neighbors import KDTree
from sklearn.metrics import mean_squared_error

sess = tf.InteractiveSession()
init_op = tf.group(tf.global_variables_initializer(), tf.local_variables_initializer())
sess.run(init_op)

case = sys.argv[2]
save_path = "/home/ldg/catkin_ws/src/nb_pose_estimation/scripts/features_out/"
test_input_image = '/home/ldg/catkin_ws/src/nb_pose_estimation/data/real/' + str(case) + str('/')
png_index = []
load_rendering_fv = []

def load_init_fv():

    file_num = len(os.walk(save_path).next()[2])
    index = 0
    #print(file_num)
    for root, dirs, files in os.walk(save_path):
        files.sort()

    for j in range(file_num):
        load_data = np.loadtxt(save_path+ str(files[j])[0:2] + '.txt')
        load_rendering_fv.append((files[j][0:2], load_data))
        png_index.append(files[j][0:2])
        index+=1
    # print(index)
    #print(png_index)


def run():

    data_num = 31
    log = open("result"+str('.txt'), "w")

    coord = tf.train.Coordinator()
    thread = tf.train.start_queue_runners(coord=coord)
    
    file_num = len(os.walk(test_input_image).next()[2])

    for root, dirs, files in os.walk(test_input_image):
        files.sort()

    #acc
    des = []
    est = []

    for j in range(file_num):
        rendering = cv2.imread(test_input_image + files[j])
        rendering = cv2.cvtColor(rendering, cv2.COLOR_BGR2RGB)
        rendering = rendering.astype(np.uint8)
        rendering = np.reshape(rendering, [1, 128, 128, 3])

        output, _ = sess.run([Net.o1, Net.o2], feed_dict={Net.x1: rendering, Net.x2: rendering})

        nearest_dist, nearest_ind = tree.query(output, k=1)
        estimated_th1 = int(str(png_index[nearest_ind[0][0]])[0:2])

        print("des= ", int(str(files[j])[1:3]))
        print("estimated= ",estimated_th1)
        des.append(int(str(files[j])[1:3]))   
        est.append(estimated_th1) 

        log.write('%f %f' % (int(str(files[j])[1:3]), estimated_th1))
        log.write('\n')

    #accuracy
    RMSE = mean_squared_error(des,est)**0.5
    print(RMSE)
    log.write('RMSE= %f' % (RMSE))
    log.close()

def init_kd_tree(vector_set, data_num, case):
    vector = []
    for i in range(data_num):
        #print(list(np.array(vector_set[0][1:2]).tolist())[0])
        vector.append(list(np.array(vector_set[i][1:2]).tolist())[0])

    vector = np.asarray(vector)
    #print(vector)
    global tree, tree2, tree3, tree4
    if case == 0:
        print(case)
        tree = KDTree(vector)
    elif case == 1:
        print(case)
        tree2 = KDTree(vector)
    elif case == 2:
        print(case)
        tree3 = KDTree(vector)
    elif case == 3:
        print(case)
        tree4 = KDTree(vector)

def main(_):
    print("Usage: python main.py image savename 'modelname'")
    global Net
    Net = nb_network_test.BDHNET()
    global saver
    saver = tf.train.Saver()
    save_model_name = "/home/ldg/catkin_ws/src/nb_pose_estimation/model/"+str(sys.argv[1])
    saver.restore(sess, save_model_name)


    load_init_fv()
    init_kd_tree(load_rendering_fv, 31, 0)


    run()


if __name__ == "__main__":
    tf.app.run(main=main, argv=[sys.argv[0]])
