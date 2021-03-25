#! /usr/bin/python
import rospy
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
from da_msg.msg import rendering_msg
from da_msg.msg import command
import numpy as np
import cv2
import sys

# Instantiate CvBridge
bridge = CvBridge()

parameter = [0,0]
pre_parameter = [0,0]
start = 0
path = "/home/ldg/Desktop/dh_data_collection_exvivo/"
save_ok = 0

def image_callback(msg): # 2dof
    global cv2_img

    #print("image callback!")
    try:
        # Convert your ROS Image message to OpenCV2
  	#print(msg)
        cv2_img = bridge.imgmsg_to_cv2(msg, "bgr8")  #mono8  cv_bridge::toCvCopy
	#rows, cols = cv2_img.shape[:2]

        #print("a")

        #r = cv2_img.astype(np.float32)
        #g = cv2_img.astype(np.float32)
        #b = cv2_img.astype(np.float32)
        #cv2_img = cv2.merge((r,g,b))
        cv2.imshow("img",cv2_img)
        cv2.waitKey(3)

    except CvBridgeError, e:
        print(e)

def parameter_callback(msg):

    global parameter, start, pre_parameter,save_ok

    parameter[0] = msg.parameter_value[0] #img
    parameter[1] = msg.parameter_value[1] #bending
    start = msg.start
   
    print(parameter[0],parameter[1])

    if(parameter[1]!=pre_parameter[1]):
        print("save ok",parameter[0],parameter[1],start)
        save_ok = 1
    else :
        print("else",parameter[0],parameter[1],start)
        save_ok = 0

    if (start == 1 and save_ok == 1) : #-9-90 ~ 90090 [name: 00000.png]
        print("save start")

        if(parameter[0]<10): #img
            if(parameter[1] < 10): #angle
                cv2.imwrite(path + str(int(parameter[0]))+'/'+str(int(0))+str(int(0))+str(int(parameter[0]))+ str(int(0))+str(int(parameter[1]))+'.png', cv2_img) #folder / image_angle
            else:
                cv2.imwrite(path + str(int(parameter[0]))+'/'+str(int(0))+str(int(0))+str(int(parameter[0]))+ str(int(parameter[1]))+'.png', cv2_img)
        elif(parameter[0]>=10 and parameter[0]<100): #img
            if(parameter[1] < 10): #angle
                cv2.imwrite(path + str(int(parameter[0]))+'/'+str(int(0))+str(int(parameter[0]))+str(int(0))+str(int(parameter[1]))+'.png', cv2_img)
            else:
                cv2.imwrite(path + str(int(parameter[0]))+'/'+str(int(0))+str(int(parameter[0]))+str(int(parameter[1]))+'.png', cv2_img)
        else:
            if(parameter[1] < 10): #angle
                cv2.imwrite(path + str(int(parameter[0]))+'/'+str(int(parameter[0]))+str(int(0))+str(int(parameter[1]))+'.png', cv2_img)
            else:
                cv2.imwrite(path + str(int(parameter[0]))+'/'+str(int(parameter[0]))+str(int(parameter[1]))+'.png', cv2_img)

        print("save end ",int(parameter[0]),int(parameter[1]))

    pre_parameter[1] = parameter[1]

def parameter_1dof_callback(msg):
   global parameter, start
   parameter[0] = msg.parameter_value[0]
   parameter[1] = msg.parameter_value[1]
   start = msg.start


def main():
    rospy.init_node('image_listener')
    image_topic = "scnn"
    da_msg_topic = "image_saver"

    print("image saver start")
    rospy.Subscriber(image_topic, Image, image_callback)
    rospy.Subscriber(da_msg_topic, command, parameter_callback)
    rospy.spin()

if __name__ == '__main__':
        main()
