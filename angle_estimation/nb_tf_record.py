from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import argparse
import os
import sys
import tensorflow as tf
import numpy as np
import cv2
import matplotlib.pyplot as plt

#input
output_path_training ="../data/txt/"
record_output_training = "../data/tfrecord/nb_training_pair_shff_noise_x.tfrecords"


writer = tf.python_io.TFRecordWriter(record_output_training)

def _bytes_feature(value):
     # input: string / byte type ,and return byte list
    return tf.train.Feature(bytes_list=tf.train.BytesList(value=[value]))

def _int64_feature(value):
    return tf.train.Feature(int64_list=tf.train.Int64List(value=[value]))

def main(_):

    inputfile = open(output_path_training+'nb_training_pair_shff.txt')

    input_var = 1
    labeling = 2
    angle_range = 60 # 1 ~ 60
    iteration = input_var * 100 # 60 20 20
    data_num = labeling * iteration * angle_range # 12000

    for i in range(data_num):
        print(i,data_num)
        data = inputfile.readline()
        list_label = int(data.split()[0])

        list_image_1 = cv2.imread(data.split()[1])
        list_image_1 = cv2.cvtColor(list_image_1, cv2.COLOR_BGR2RGB)
        list_image_1 = list_image_1.astype(np.uint8) # 0 ~ 255

        list_image_2 = cv2.imread(data.split()[2])
        list_image_2 = cv2.cvtColor(list_image_2, cv2.COLOR_BGR2RGB)
        list_image_2 = list_image_2.astype(np.uint8)


        instance = tf.train.Example(features=tf.train.Features(feature={
            'camera': _bytes_feature(tf.compat.as_bytes(list_image_1.tostring())),
            'rendering': _bytes_feature(tf.compat.as_bytes(list_image_2.tostring())),
            'label': _int64_feature(list_label)}))
        
        writer.write(instance.SerializeToString())
    writer.close()


if __name__ == "__main__":
    print("Usage===============")
    print("Second step : python bdh_tf_record.py")
    print("====================")
    tf.app.run(main=main, argv=[sys.argv[0]])
