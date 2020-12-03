import tensorflow as tf


class BDHNET:

    def __init__(self):
        self.x1=tf.placeholder(tf.float32,[None,128,128,3])
        self.x2=tf.placeholder(tf.float32,[None,128,128,3])
        self.phase = tf.placeholder(tf.bool, name='phase')

        with tf.variable_scope("BDHNET") as scope:
            self.o1=self.mynet_cam(self.x1)
            scope.reuse_variables()  #uncomment this line for siamese-network, comment this line for pseudo-siamese network
            self.o2=self.mynet_cam(self.x2)
        self.y_ = tf.placeholder(tf.float32,[None])
        self.loss = self.one_shot_test_binsary_cross_entropy_loss()
        

    def mynet_cam(self,input_):

        regularizer = tf.contrib.layers.l2_regularizer(scale=0.01)
        #tf.contrib.layers.variance_scaling_initializer()  
        with tf.name_scope("model"):
		with tf.variable_scope("conv1") as scope:
			net = tf.contrib.layers.conv2d(input_, 32, [5, 5],stride=2,weights_regularizer=regularizer, activation_fn=tf.nn.relu, padding='SAME',
		        weights_initializer=tf.contrib.layers.xavier_initializer_conv2d(),scope=scope)
                        #net = tf.contrib.layers.batch_norm(net, center=True, scale=True, is_training=self.phase, scope='bn')
                        #net = tf.nn.relu(net, 'relu')
			net = tf.contrib.layers.max_pool2d(net, [2, 2], padding='SAME')

		with tf.variable_scope("conv2") as scope:
			net = tf.contrib.layers.conv2d(net, 64, [3, 3], stride=2,weights_regularizer=regularizer,activation_fn=tf.nn.relu, padding='SAME',
		        weights_initializer=tf.contrib.layers.xavier_initializer_conv2d(),scope=scope)
                        #net = tf.contrib.layers.batch_norm(net, center=True, scale=True, is_training=self.phase, scope='bn')
                        #net = tf.nn.relu(net, 'relu')
			net = tf.contrib.layers.max_pool2d(net, [2, 2], padding='SAME')

		with tf.variable_scope("conv3") as scope:
			net = tf.contrib.layers.conv2d(net, 128, [3, 3], stride=2,weights_regularizer=regularizer,activation_fn=tf.nn.relu, padding='SAME',
		        weights_initializer=tf.contrib.layers.xavier_initializer_conv2d(),scope=scope)
                        #net = tf.contrib.layers.batch_norm(net, center=True, scale=True, is_training=self.phase, scope='bn')
                        #net = tf.nn.relu(net, 'relu')
			net = tf.contrib.layers.max_pool2d(net, [2, 2], padding='SAME')

		with tf.variable_scope("conv4") as scope:
			net = tf.contrib.layers.conv2d(net, 256, [3, 3], stride=2,activation_fn=tf.nn.relu, padding='SAME',
		        weights_initializer=tf.contrib.layers.xavier_initializer_conv2d(),scope=scope)
                        #net = tf.contrib.layers.batch_norm(net, center=True, scale=True, is_training=self.phase, scope='bn')
                        #net = tf.nn.relu(net, 'relu')
			net = tf.contrib.layers.max_pool2d(net, [2, 2], padding='SAME')

		with tf.variable_scope("conv5") as scope:
			net = tf.contrib.layers.conv2d(net, 256, [3, 3], activation_fn=tf.nn.relu, padding='SAME',
		        weights_initializer=tf.contrib.layers.xavier_initializer_conv2d(),scope=scope)
			#net = tf.contrib.layers.max_pool2d(net, [2, 2], padding='SAME') 
		
                net = tf.contrib.layers.flatten(net)

                with tf.variable_scope("fc1") as scope:
                        net = tf.contrib.layers.fully_connected(net, 256, activation_fn=tf.nn.sigmoid, 
                        weights_initializer=tf.contrib.layers.xavier_initializer(),scope=scope,reuse=tf.AUTO_REUSE)
                        #net = tf.contrib.layers.dropout(net,keep_prob=0.5)
                
                with tf.variable_scope("fc2") as scope:
                        net = tf.contrib.layers.fully_connected(net, 256, activation_fn=tf.nn.sigmoid, 
                        weights_initializer=tf.contrib.layers.xavier_initializer(),scope=scope,reuse=tf.AUTO_REUSE)
                        #net = tf.contrib.layers.dropout(net,keep_prob=0.5)
                
        return net


    def one_shot_test_binsary_cross_entropy_loss(self):   #contrastive loss

        # yi*||CNN(p1i)-CNN(p2i)||^2 + (1-yi)*max(0, C-||CNN(p1i)-CNN(p2i)||^2)

        margin = 1.0
        labels_yi = self.y_
        labels_1_yi = tf.subtract(1.0, self.y_, name="1-yi")          # labels_ = !labels;

        eucd2 = tf.pow(tf.subtract(self.o1, self.o2), 2)
        eucd2 = tf.reduce_sum(eucd2, 1)
        eucd = tf.sqrt(eucd2+1e-6, name="eucd")
        C = tf.constant(margin, name="C")
        pos = tf.multiply(labels_yi, eucd2, name="yi_x_eucd2")
        neg = tf.multiply(labels_1_yi, tf.pow(tf.maximum(tf.subtract(C, eucd), 0), 2), name="Nyi_x_C-eucd_xx_2")
        losses = tf.add(pos, neg, name="losses")
        loss = tf.reduce_mean(losses, name="loss")
        #regularizer
        l2_loss = tf.losses.get_regularization_loss()
        loss += l2_loss
        #beta = 0.01
        #regularizers = tf.nn.l2_loss(weight_1) + tf.nn.l2_loss(weight_2) + tf.nn.l2_loss(weight_3) + tf.nn.l2_loss(weight_4) + tf.nn.l2_loss(weight_5)
        #loss = tf.reduce_mean(loss + beta*regularizers)
       
        return loss
