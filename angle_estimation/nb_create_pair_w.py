import os
import random

# resize img
filepath_annotation = "./data/resize_128/real/"
filepath_rendering = './data/resize_128/label/'

output_txt_path = "./txt/"


def create_pairset(anno_path, render_path):

    file_num = len(os.walk(anno_path).next()[1]) #5

    for i in range(file_num):

        file_N = len(os.walk(anno_path+str(i)).next()[2])

        for root, dirs, files_1 in os.walk(anno_path+str(i)):
            files_1.sort() #order -9 ~ 90

        for root, dirs, files_2 in os.walk(render_path):
            files_2.sort()

        num = 1
        iteration = 100 # how many datasets are you create?

        for j in range(file_N):
             while num <= iteration:
                write_txt_true(files_1, files_2, i, j) # create true dataset#

                write_txt_false(files_1, files_2, i, j) #create false dataset

                num += 1
                print(i+1,j,file_N,num)
             num = 1
        print("Completed")

def write_txt_false(files_1, files_2, i ,j):
    text_file.write("0")
    text_file.write(" ")
    text_file.write(filepath_annotation + str(i) + str('/') + str(files_1[j]))
    text_file.write(" ")

    new_num = random.randrange(0, 61) 
    while(new_num == j):
            new_num = random.randrange(0, 61)
            print("same parameter")
            if (new_num != j):
                break
    index = int(new_num)
    print(index)
    text_file.write(filepath_rendering + str(files_2[index]))
    text_file.write("\n")

    #false data test
    false_data_test_file.write(str(files_1[j]))
    false_data_test_file.write(" ")
    false_data_test_file.write(str(files_2[index]))
    false_data_test_file.write("\n")    



def write_txt_true(files_1, files_2, i, j):
    text_file.write("1")
    text_file.write(" ")
    text_file.write(filepath_annotation + str(i) + str('/') + str(files_1[j]))
    text_file.write(" ")
    text_file.write(filepath_rendering + str(files_2[j]))
    text_file.write("\n")

if __name__ == '__main__':
  print("Usage===============")
  print("First step. pairset.py")
  print("====================")

  global text_file, false_data_test_file

  text_file = open(output_txt_path + "nb_training_pair.txt", "w")
  false_data_test_file= open(output_txt_path + "nb_false_data_test_file.txt", "w")
 
  create_pairset(filepath_annotation, filepath_rendering)
  text_file.close()
