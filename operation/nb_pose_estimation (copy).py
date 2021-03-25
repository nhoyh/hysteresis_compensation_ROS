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

import nb_network
from sklearn.neighbors import KDTree

# Instantiate CvBridge
bridge = CvBridge()
pub = rospy.Publisher('scnn_out',vision_estimation,queue_size=1)


#for scnn
sess = tf.InteractiveSession()
init_op = tf.group(tf.global_variables_initializer(), tf.local_variables_initializer())
sess.run(init_op)

coord = tf.train.Coordinator()
thread = tf.train.start_queue_runners(coord=coord)

save_path = "/home/ldg/catkin_ws/src/nb_pose_estimation/scnn/features_out/"

load_rendering_fv = []
png_index = []

pre_estimated_th1 = 0


def image_callback(msg):
    global cv2_img
    try:   
        # Convert your ROS Image message to OpenCV2
        cv2_img = bridge.imgmsg_to_cv2(msg, desired_encoding='passthrough')  #mono8  cv_bridge::toCvCopy
        cv2_img = cv2.normalize(cv2_img, None, alpha = 0, beta = 255, norm_type = cv2.NORM_MINMAX, dtype = cv2.CV_32F)
        cv2_img = cv2_img.astype(np.uint8)
   
        cv2_img = cv2.resize(cv2_img, dsize=(128, 128), interpolation=cv2.INTER_AREA)
        cv2_img = np.reshape(cv2_img,[-1,128,128,3])

        #cv2.imshow('cv2',cv2_img)
        #cv2.waitKey(3)

        run_processing(cv2_img)
        
    except CvBridgeError, e:
        print(e)

def run_processing(input_image):

    ti= time.time()
    estimated_th = Retrieval_processing(input_image)

    msg = vision_estimation()
    msg.parameter_value = [estimated_th]

    pub.publish(msg)
    tf = time.time()

    print("th=%d ,time = %f" %(estimated_th, tf-ti))


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
    global pre_estimated_th1

    output, _ = sess.run([Net.o1, Net.o2], feed_dict={Net.x1: test_img, Net.x2: test_img})
    nearest_dist, nearest_ind = tree.query(output, k=1)
    # print(nearest_ind)
    # print("desired=%s ,index=%s" % (str(files[j])[0:5], png_index[nearest_ind[0][0]]))
    estimated_th1 = int(str(png_index[nearest_ind[0][0]])[0:2])

    pre_estimated_th1 = estimated_th1

    return estimated_th1

def init_kd_tree(vector_set, data_num, case):
    vector = []
    for i in range(data_num):
        #print(list(np.array(vector_set[0][1:2]).tolist())[0])
        vector.append(list(np.array(vector_set[i][1:2]).tolist())[0])
    vector = np.asarray(vector)

    global tree
    tree = KDTree(vector)


def main():
    rospy.init_node('scnn_pose_estimation')
    image_topic = "gan_img"

    #init net, saver
    global Net
    Net = nb_network.BDHNET()

    global saver
    saver = tf.train.Saver()

    #global sfk
    #sfk = sensor_fussion()
    save_model_name = "/home/ldg/catkin_ws/src/nb_pose_estimation/scnn/model/bdhnet-200"
    saver.restore(sess, save_model_name)


    print("scnn pose estimation start")
    rospy.Subscriber(image_topic, Image, image_callback)
    rospy.spin()

if __name__ == '__main__':
    load_init_fv()

    data_num = len(load_rendering_fv)
    print(data_num)

    init_kd_tree(load_rendering_fv, data_num, 0)

    main()






