from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import argparse
import os
import sys
import numpy as np
import cv2
import matplotlib.pyplot as plt
import random

#input
output_path_training =  "./txt/"

def run():

    inputfile = open(output_path_training+'nb_training_pair.txt')

    #data_num = len(inputfile.readlines()) 
    #print(data_num)
    data = inputfile.readlines()
    random.shuffle(data)
    open(output_path_training+'nb_training_pair_shff.txt', 'w').writelines(data)
    

if __name__ == "__main__":
    print("Usage===============")
    print("Second step : python data shuffle.py")
    print("====================")
    run()
