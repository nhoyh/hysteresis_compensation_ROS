import os
import numpy as np
import cv2
from PIL import Image
import matplotlib.pyplot as plt
import random

input_img_path = "../data/real/"
output_img_path = "../data/real_noise/"
output_img_binary = "../data/binary/"
output_img_test="../data/testset_diffnoise/1/"

def sp_noise(image):

    output = np.zeros(image.shape,np.uint8)
    for i in range(image.shape[0]): #img height
        for j in range(image.shape[1]): #img width
            rdn = random.randrange(1,8)
            if rdn > 6:
                #adding noise box
                if(i+1<image.shape[0] and j+1<image.shape[1]):
		   #output[i-1][j-1] = 0
                   #output[i-1][j] = 0
		   #output[i-1][j+1] = 0
                   #output[i][j-1] = 0
		   output[i][j] = 0
                   #output[i][j+1] = 0
                   #output[i+1][j-1] = 0
		   #output[i+1][j] = 0
                   #output[i+1][j+1] = 0 
            else:
                output[i][j] = image[i][j]
    return output


def guassian_noise(img):
    mean = 0.0
    std  = 1.0
    noisy_img = img + np.random.normal(mean,std,img.shape)
    noisy_img_clipped = np.clip(noisy_img,0,255)
    return  noisy_img_clipped

def create_folder(work_dir):

    try:
        if not os.path.isdir(work_dir):
            os.mkdir(work_dir)
    except OSError as e:
            print("Failed to create directory!!!!!")
            raise


def create_binary_img():

    file_num = len(os.walk(input_img_path).next()[2])
    
    for root, dirs, files in os.walk(input_img_path):
        files.sort()

    for j in range(file_num):
        img = cv2.imread(input_img_path+files[j])
        gray = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
    	ret, thresh = cv2.threshold(gray,100, 255, cv2.THRESH_BINARY_INV+ cv2.THRESH_OTSU)

        cv2.imwrite(output_img_binary+ files[j],thresh)

def create_noise_img(path):

    file_num = len(os.walk(output_img_binary).next()[2])
    print(file_num)

    for root, dirs, files in os.walk(output_img_binary):
        files.sort()

    for j in range(file_num):
        img = cv2.imread(output_img_binary+files[j])
        #img = guassian_noise(img)
        img = sp_noise(img)
        #print(img.shape)
	
	#plt.imshow(thresh,'gray')
        #plt.show()
        cv2.imwrite(path+ files[j],img)

    

if __name__ == '__main__':

   print("create_binary_img")
   #create_binary_img()
   create_noise_img(output_img_test)



