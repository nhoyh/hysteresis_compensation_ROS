import tensorflow as tf
import os
import numpy as np
import nb_network
import sys
import matplotlib.pyplot as plt

#parameter definition
flags = tf.app.flags
FLAGS = flags.FLAGS
flags.DEFINE_integer('batch_size', 200, 'Batch size.')
flags.DEFINE_integer('train_iter', 1000000, 'Total training iter')
flags.DEFINE_integer('training_epochs', 500, 'epoch')
flags.DEFINE_integer('img_size', 128, 'image size')
flags.DEFINE_float('learning_rate', 0.00003, 'learning rate')

def read_test():
    init_op = tf.group(tf.compat.v1.global_variables_initializer())
    sess = tf.compat.v1.InteractiveSession()
    sess.run(init_op) #transfer tensor sets

    camera, rendering, label = database_read()
    coord = tf.train.Coordinator()
    threads = tf.train.start_queue_runners(coord=coord)

    for i in range(3):
        test1, test2, test3 = sess.run([camera, rendering, label])

        plt.imshow(test1.astype(np.uint8))
    plt.show()

def database_read():
    filename_queue = tf.compat.v1.train.string_input_producer([tfrecords_filename])

    reader = tf.TFRecordReader()
    _, serialized_example = reader.read(filename_queue)

    features =  tf.io.parse_single_example(serialized_example, features={
        'camera': tf.io.FixedLenFeature([], tf.string),
        'rendering': tf.io.FixedLenFeature([], tf.string),
        'label': tf.io.FixedLenFeature([], tf.int64)})

    label = tf.cast(features['label'], tf.int64) #change type
    camera = tf.decode_raw(features['camera'], tf.uint8)
    rendering = tf.decode_raw(features['rendering'], tf.uint8)

    camera = tf.reshape(camera, [FLAGS.img_size, FLAGS.img_size, 3])
    rendering = tf.reshape(rendering, [FLAGS.img_size, FLAGS.img_size, 3])
    label = tf.reshape(label, [1])

    return camera, rendering, label

def main(_):
    #data sample test
    read_test()

    #parsing data
    camera, rendering, label = database_read()

    min_after_dequeue_ = 10000
    capacity_ = min_after_dequeue_ + 3 * FLAGS.batch_size

    camera_batch, rendering_batch, label_batch = tf.train.shuffle_batch([camera, rendering, label],batch_size=FLAGS.batch_size,num_threads=8,capacity=capacity_,min_after_dequeue=min_after_dequeue_)

    camera_batch = tf.compat.v1.placeholder_with_default(camera_batch, shape=(FLAGS.batch_size, FLAGS.img_size,FLAGS.img_size,3)) #200, 128,128,3
    rendering_batch = tf.compat.v1.placeholder_with_default(rendering_batch, shape=(FLAGS.batch_size, FLAGS.img_size,FLAGS.img_size,3))
    label_batch = tf.reshape(label_batch, [FLAGS.batch_size])
    
    #initialize
    global_step = tf.Variable(0, trainable=False)
    sess = tf.compat.v1.InteractiveSession()
    Net=nb_network.BDHNET()
    learning_rate = FLAGS.learning_rate

    # Define your exponentially decaying learning rate
    lr = tf.compat.v1.train.exponential_decay(
        learning_rate= learning_rate,
        global_step= global_step,
        decay_steps= FLAGS.train_iter,
        decay_rate=0.95,
        staircase=True)

    optimizer = tf.compat.v1.train.AdamOptimizer(lr).minimize(Net.loss)
    # optimizer = tf.train.GradientDescentOptimizer(learning_rate).minimize(Net.loss)  # learning rate
    saver = tf.compat.v1.train.Saver(max_to_keep=1000)

    coord = tf.train.Coordinator()
    threads = tf.train.start_queue_runners(coord=coord)

    # network
    sess.run(tf.compat.v1.global_variables_initializer())

    # setup tensorboard
    tf.compat.v1.summary.scalar('step', global_step)
    tf.compat.v1.summary.scalar('loss', Net.loss)
    for var in tf.compat.v1.trainable_variables():
        tf.compat.v1.summary.histogram(var.op.name, var)
    merged = tf.compat.v1.summary.merge_all()
    writer = tf.compat.v1.summary.FileWriter('train.log', sess.graph)

    
    log = open("log.txt", "w")
    itr = 0

    for epoch in range(FLAGS.training_epochs): 
        total_batch = int(12000/FLAGS.batch_size) 

        for i in range(total_batch):
           D1, D2, L = sess.run([camera_batch, rendering_batch, label_batch])
           _, loss, summary_str = sess.run([optimizer, Net.loss, merged],
                                        feed_dict={Net.x1: D1, Net.x2: D2, Net.y_: L})

           writer.add_summary(summary_str, itr) #to tensorboard
	   itr +=1
           if (i % 1 == 0):
              print("epoch=%d, iteration = %d,loss = %f" % (epoch,i, loss))
              log.write('%d, %d,  %f' % (epoch,i, loss))
        if(epoch%5 ==0):  
           saver.save(sess, './model/bdhnet', global_step=epoch)
    log.close()



if __name__ == '__main__':
    print("=====usage=====")
    global tfrecords_filename
    tfrecords_filename= "../data/tfrecord/nb_training_pair_shff_noise_x.tfrecords"
    tf.compat.v1.app.run(main=main, argv=[sys.argv[0]])
