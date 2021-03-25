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

#for seg
from glob import glob
import scipy.misc
import numpy as np
import matplotlib.pyplot as plt
try:
    _imread = scipy.misc.imread
except AttributeError:
    from imageio import imread as _imread

import cv2
import sys
import operator
import time

# Instantiate CvBridge
bridge = CvBridge()

def image_callback(msg):

    try:
        global cv2_img
        # Convert your ROS Image message to OpenCV2
        cv2_img = bridge.imgmsg_to_cv2(msg, "bgr8")  #mono8      
        run_processing(cv2_img)
        
    except CvBridgeError, e:
        print(e)



def run_processing(input_image):


    #gan start
    img_resize = cv2.resize(cv2_img, dsize=(256, 256), interpolation=cv2.INTER_AREA)
    img_resize_guide = cv2.add(img_resize,guide_img)
    cv2.imshow("img",img_resize_guide)
    cv2.waitKey(3)
    img_resize = np.reshape(img_resize,[-1,256,256,3])

    ti= time.time()
    feed_dict = {test_A:img_resize}
    result = sess.run(target_tensor,feed_dict)

    output_img = np.reshape(result,[256,256,3])
    #img_resize_guide = cv2.add(output_img,guide_img)
    tf = time.time()

    #print(output_img.shape)
    #pub img
    img_pub.publish(bridge.cv2_to_imgmsg(output_img, encoding='passthrough'))

    #show img
    #print(guide_img.shape)
    #img_resize_guide = cv2.add(output_img,guide_img)
    cv2.imshow("result",output_img)
    cv2.waitKey(3)

    print("time = %f" %(tf-ti))




def load_test_data(image_path, fine_size=256):
    img = imread(image_path)
    img = scipy.misc.imresize(img, [fine_size, fine_size])
    img = img/127.5 - 1
    return img

def imread(path, is_grayscale = False):
    if (is_grayscale):
        return _imread(path, flatten=True).astype(np.float)
    else:
        return _imread(path, mode='RGB').astype(np.float)


def main(_):
    rospy.init_node('image_listener')
    image_topic = "gan_segmentation"

    global test_A, target_tensor, sess


    sess=tf.Session()   
    init_op = tf.global_variables_initializer()
    sess.run(init_op) 
    saver = tf.train.import_meta_graph('checkpoint/cyclegan.model-42662.meta')
    saver.restore(sess,tf.train.latest_checkpoint('./checkpoint'))

    graph = tf.get_default_graph()
    test_A = graph.get_tensor_by_name("test_A:0")
    target_tensor = graph.get_tensor_by_name("generatorA2B_2/Tanh:0")

    
    #guide image  
    global guide_img
    guide_img = cv2.imread('guide.png')#,cv2.IMREAD_COLOR)
    #print(guide_img.shape)
    #cv2.imshow("guide_img",guide_img)
    #cv2.waitKey(3)

    #pub img
    global img_pub
    img_pub = rospy.Publisher("gan_img",Image)

    #subscriber callback
    rospy.Subscriber(image_topic, Image, image_callback)
    rospy.spin()

if __name__ == '__main__':

    config = tf.ConfigProto()
    config.gpu_options.per_process_gpu_memory_fraction = 0.4
    tf.keras.backend.set_session(tf.Session(config=config))
    tf.app.run(main=main, argv=[sys.argv[0]])
