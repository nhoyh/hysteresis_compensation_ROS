#! /usr/bin/python
# 7.12.2020 created by young-hoon nho,Donghoon Baek nhoyh1@gmail.com

import rospy
import numpy as np
import os
import tensorflow as tf
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
from da_msg.msg import command
from da_msg.msg import vision_estimation

import cv2
import sys
import operator
import time
from skimage.transform import resize # nhoyh..: pip install scikit-image

import nb_network_test
from sklearn.neighbors import KDTree

#for kalman
from numpy import matrix, diag, random
from helper_utils import MovingAverage
from helper_utils import Logger
from helper_utils import KalmanPlotter
from kalman_filter import KalmanFilter

# Instantiate CvBridge
bridge = CvBridge()
pub = rospy.Publisher('scnn_out',vision_estimation,queue_size=1)


#for scnn
sess = tf.InteractiveSession()
init_op = tf.group(tf.global_variables_initializer(), tf.local_variables_initializer())
sess.run(init_op)

coord = tf.train.Coordinator()
thread = tf.train.start_queue_runners(coord=coord)

save_path = "/home/ldg/catkin_ws/src/nb_pose_estimation/features_out/"

load_rendering_fv = []
png_index = []

estimated_th_scnn = 0
val_bef1 = 0
val_bef2 = 0
val_bef3 = 0
val_bef4 = 0
val_bef5 = 0
val_bef6 = 0
val_bef7 = 0
val_bef8 = 0
val_bef9 = 0
val_bef10 = 0
val_bef11 = 0
val_bef12 = 0
val_bef13 = 0
val_bef14 = 0
val_bef15 = 0
val_bef16 = 0
val_bef17 = 0
val_bef18 = 0
val_bef19 = 0
val_bef20 = 0
init_estimation = True;


def image_callback(msg):
    global cv2_img, init_estimation
    try:   
        # Convert your ROS Image message to OpenCV2
        cv2_img = bridge.imgmsg_to_cv2(msg, desired_encoding='passthrough')  #mono8  cv_bridge::toCvCopy
        #a1 = cv2_img
        cv2_img = cv2.normalize(cv2_img, None, alpha = 0, beta = 255, norm_type = cv2.NORM_MINMAX, dtype = cv2.CV_32F)
        #a2 = cv2_img
        cv2_img = cv2_img.astype(np.uint8)
        #a3 = cv2_img

        #small_image = a3.reshape((128, 2,128,2, 3)).max(3).max(1)
        #smaller_img1 = a3[::2, ::2]

        cv2_img = cv2.resize(cv2_img, dsize=(128, 128)) #, interpolation=cv2.INTER_AREA)
        cv2_img = cv2.cvtColor(cv2_img, cv2.COLOR_BGR2RGB)
        cv2_img = cv2_img.astype(np.uint8)
        cv2_img = np.reshape(cv2_img, [1, 128, 128, 3])
        #a4 = cv2_img
        #cv2_img = np.reshape(cv2_img,[-1,128,128,3])
        #a5 = cv2_img

        #cv2.imshow('cv2',a4)
        #cv2.waitKey(3)
        
        #run_processing(cv2_img)
        if init_estimation == True:
            estimated_th = Retrieval_processing(cv2_img)
        
    except CvBridgeError, e:
        print(e)


def average_filter(val):
    global val_bef1, val_bef2, val_bef3, val_bef4, val_bef5
#,val_bef6, val_bef7, val_bef8, val_bef9, val_bef10,val_bef11, val_bef12, val_bef13, val_bef14, val_bef15, val_bef16, val_bef17, val_bef18, val_bef19, val_bef20

    #val_bef20 = val_bef19
    #val_bef19 = val_bef18
    #val_bef18 = val_bef17
    #val_bef17 = val_bef16
    #val_bef16 = val_bef15
    #val_bef15 = val_bef14
    #val_bef14 = val_bef13
    #val_bef13 = val_bef12
    #val_bef12 = val_bef11
    #val_bef11 = val_bef10
    #val_bef10 = val_bef9
    #val_bef9 = val_bef8
    #val_bef8 = val_bef7
    #val_bef7 = val_bef6
    #val_bef6 = val_bef5
    val_bef5 = val_bef4
    val_bef4 = val_bef3
    val_bef3 = val_bef2
    val_bef2 = val_bef1
    val_bef1 = val 
    avg = (val_bef1+ val_bef2+ val_bef3+ val_bef4+ val_bef5) /5
#+val_bef6+ val_bef7+ val_bef8+ val_bef9+ val_bef10+val_bef11+ val_bef12+ val_bef13+ val_bef14+ val_bef15+ val_bef16+ val_bef17+ val_bef18+ val_bef19+ val_bef20) /20
#    print(val_bef5, val_bef4, val_bef3, val_bef2, val_bef1)

    if val_bef5 is 0: # val_bef5 is 0:
        return val
    else:  
        return avg



def run_processing_kalman(msg):
    global init_estimation
    init_estimation = False
    ti= time.time()

    w = matrix(random.multivariate_normal([0.0, 0.0], sfk.Q)).T
    # Predict
    (sfk.X, sfk.P) = sfk.kf.predict(sfk.X, sfk.P, w)
    # Update vision
    (sfk.X, sfk.P) = sfk.kf.update(sfk.X, sfk.P, sfk.Z1)

    offset = 0

    if np.shape(cv2_img)[1] is not 128:
        pass
    elif np.shape(cv2_img)[0] is 128:
        pass
    else:
        estimated_th = Retrieval_processing(cv2_img)
        sfk.Z1 = matrix([[estimated_th], [0.0]])
        #af_1 = average_filter(estimated_th)
        #af_2 = average_filter(sfk.X[0, 0])
        #af_2  =   
        pub_msg = vision_estimation()
        pub_msg.parameter_value = [sfk.X[0, 0]+offset, sfk.X[0, 0]+offset]
        #pub_msg.parameter_value = [estimated_th+offset, af_1+offset]
        pub.publish(pub_msg)

        tf = time.time()
        print("th=%f, f_th=%f ,time = %f" %(estimated_th+offset, sfk.X[0, 0]+offset, tf-ti))
        #print("th=%f, f_th=%f ,time = %f" %(estimated_th, af_1+offset, tf-ti))



def load_init_fv():

    file_num = len(os.walk(save_path).next()[2])
    index = 0
    print(file_num)
    for root, dirs, files in os.walk(save_path):
        files.sort()

    for j in range(file_num):
        load_data = np.loadtxt(save_path+ str(files[j])[0:2] + '.txt')
        load_rendering_fv.append((files[j][0:2], load_data))
        png_index.append(files[j][0:2])
        index+=1
    # print(index)
    print(png_index)

def Retrieval_processing(test_img):

    global estimated_th_scnn
    output, _ = sess.run([Net.o1, Net.o2], feed_dict={Net.x1: test_img, Net.x2: test_img})
    nearest_dist, nearest_ind = tree.query(output, k=1)
    estimated_th_scnn = int(str(png_index[nearest_ind[0][0]])[0:2])
    
    if init_estimation == True:
        print("estimated th:",estimated_th_scnn, ", dist:",nearest_dist)


    #if no kalman
    #pub_msg = vision_estimation()
    #pub_msg.parameter_value = [estimated_th_scnn, estimated_th_scnn]
    #pub.publish(pub_msg)


    return estimated_th_scnn

def init_kd_tree(vector_set, data_num, case):
    vector = []
    for i in range(data_num):
        #print(list(np.array(vector_set[0][1:2]).tolist())[0])
        vector.append(list(np.array(vector_set[i][1:2]).tolist())[0])
    vector = np.asarray(vector)

    global tree
    tree = KDTree(vector)

class Kalman_filter:
    
    def __init__(self):
        # Time step size
        self.dt = 0.1
        # Standard deviation of random accelerations
        self.sigma_a = 0.01
        # Standard deviation of observations
        self.sigma_z = 0.1 #CAM

        # State vector: [[Position], [velocity]]
        self.X = matrix([[0.0], [0.0]])
        # Initial state covariance
        self.P = diag((0.0, 0.0))
        # Acceleration model
        self.G = matrix([[(self.dt ** 2) / 2], [self.dt]])
        # State transition model
        self.F = matrix([[1, self.dt], [0, 1]])
        # Process noise covariance matrix
        self.Q = self.G * (self.G.T) * self.sigma_a ** 2

        #sensor 1
        self.Z1 = matrix([[0.0], [0.0]])
        self.H1 = matrix([[1, 0], [0, 0]])
        self.R1 = matrix([[self.sigma_z ** 2, 0], [0, 1]])

        # Initialise the filter
        self.kf = KalmanFilter(self.X, self.P, self.F, self.Q, self.Z1, self.H1, self.R1)


def main():
    rospy.init_node('scnn_pose_estimation')
    image_topic = "gan_img"
    des_pos_topic = "des_pos"

    #init net, saver
    global Net
    Net = nb_network_test.BDHNET()

    global saver
    saver = tf.train.Saver()

    global sfk
    sfk = Kalman_filter()
    save_model_name = "/home/ldg/catkin_ws/src/nb_pose_estimation/model/"+str(sys.argv[1]) #bdhnet-200"
    saver.restore(sess, save_model_name)


    print("scnn pose estimation start")

    rospy.Subscriber(des_pos_topic, command, run_processing_kalman)
    rospy.Subscriber(image_topic, Image, image_callback)
    rospy.spin()

if __name__ == '__main__':
    load_init_fv()

    data_num = len(load_rendering_fv)
    print(data_num)

    init_kd_tree(load_rendering_fv, data_num, 0)

    main()






