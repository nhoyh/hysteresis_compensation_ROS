import os
import random
import numpy as np
import cv2
import matplotlib.pyplot as plt
from PIL import Image


input_img = "./data/original/real/"
input_label = "./data/original/label/"

output_img = "./data/resize_128/real/"
output_label = "./data/resize_128/label/"

'''
def create_foler(work_dir):

    for i in range(51):
        folder_name = str(i + start_num)
        try:
            if not os.path.isdir(work_dir + folder_name):
                os.mkdir(work_dir + folder_name)
        except OSError as e:
            if e.errno != errno.EEXIST:
                print("Failed to create directory!!!!!")
                raise

'''
def resize_imge(filepath,output_path):

    file_N = len(os.walk(filepath).next()[1])
    print(file_N)

    for i in range(file_N):

        file_num = len(os.walk(filepath+str(i)+str("/")).next()[2])

        for root, dirs, files in os.walk(filepath+str(i)+str("/")):
            files.sort()

        for j in range(file_num):
            im = Image.open(filepath+str(i)+str("/")+files[j])
            im = im.resize((128, 128), Image.ANTIALIAS)
            
            #plt.imshow(im)
            #plt.show()
            im.save(output_path+str(i)+str("/")+files[j])
            print(i, j+1)

def resize_image_label(filepath,output_path):
    file_num = len(os.walk(filepath).next()[2])
    print(file_num)

    for root, dirs, files in os.walk(filepath):
        files.sort()
    
    for i in range(file_num):
        im = cv2.imread(filepath + files[i])
        im = im[0:1920/2,0:1080]
        im = cv2.resize(im, (128, 128),  interpolation=cv2.INTER_CUBIC)

        cv2.imwrite(output_path+ files[i],im)



if __name__ == '__main__':
    print("Usage===============")
    print("First step. img resize.py")
    print("====================")

    #create_foler(output_reize_label)

    #resize_image(original_img_path_label, output_reize_label)
    #resize_image_real(original_img_path_label, output_reize_label)
    resize_imge(input_img, output_img)
    resize_image_label(input_label,output_label)
