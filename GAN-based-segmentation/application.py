# 12.Dec.2020 created by young-hoon nho, nhoyh1@gmail.com
# only using with python3
import tensorflow as tf
from glob import glob
import scipy.misc
import numpy as np
import matplotlib.pyplot as plt
try:
    _imread = scipy.misc.imread
except AttributeError:
    from imageio import imread as _imread

def run():
    sess=tf.Session()   
    init_op = tf.global_variables_initializer()
    sess.run(init_op) 
    saver = tf.train.import_meta_graph('checkpoint/cyclegan.model-42662.meta')
    saver.restore(sess,tf.train.latest_checkpoint('./checkpoint'))

    graph = tf.get_default_graph()
    test_A = graph.get_tensor_by_name("test_A:0")
    target_tensor = graph.get_tensor_by_name("generatorA2B_2/Tanh:0")

    sample_files = glob('./datasets/0931.png')

    sample_image = [load_test_data(sample_files[0], 256)]
    sample_image = np.array(sample_image).astype(np.float32)

    feed_dict = {test_A:sample_image}
    result = sess.run(target_tensor,feed_dict)
#    print(result.shape)
    aa = np.reshape(result,[256,256,3])

    plt.figure()
    plt.imshow(((aa+1)*255/2).astype(np.uint8)) # -1 to 1 scale to 0 to 255
    s = 'output/segmented.png' 
    plt.axis("off")
    plt.savefig(s,bbox_inches='tight', pad_inches=0)
    plt.close()
    print('Generated image is saved in output folder!')


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

if __name__ == '__main__':
    run()
