#!/usr/bin/env python
import sys
import numpy as np
import os
import tensorflow as tf
import nb_network_test
from scipy.spatial import distance
import cv2


work_dir = "./features_out/"
filepath_rendering = '../data/label/'


sess = tf.InteractiveSession() #tf.Session() ->> connection among python object, data, and memory
init_op = tf.group(tf.global_variables_initializer(), tf.local_variables_initializer())
sess.run(init_op) #operation execution


def run():

    file_num = len(os.walk(filepath_rendering).next()[2])

    for root, dirs, files in os.walk(filepath_rendering):
        files.sort()

    for i in range(file_num):
               
        rendering = cv2.imread(filepath_rendering+ files[i])
        rendering = np.reshape(rendering, [1, 128, 128, 3])

        coord = tf.train.Coordinator()
        thread = tf.compat.v1.train.start_queue_runners(coord=coord)

        output1, output2 = sess.run([Net.o1, Net.o2], feed_dict={Net.x1: rendering, Net.x2: rendering}) #insertioning only rendering file into trained-network model

        np.savetxt(work_dir  + str(files[i])[0:2] + '.txt', output1, delimiter=' ', fmt='%1.4f')
        
def main(_):
  print("Usage: python main.py image savename 'modelname'")

  global Net
  Net = nb_network_test.BDHNET()

  global saver
  saver = tf.compat.v1.train.Saver()
  save_model_name = "../model/"+str(sys.argv[1])
  saver.restore(sess, save_model_name)

  coord = tf.train.Coordinator() #thread control 
  thread = tf.train.start_queue_runners(coord=coord) #multi thread generator enqueuing tensor to same queue


  run()


if __name__ == "__main__":
  tf.app.run(main=main, argv=[sys.argv[0]])
