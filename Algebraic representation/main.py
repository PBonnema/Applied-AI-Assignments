import tensorflow as tf
from tensorflow.keras.datasets import mnist
from tensorflow.keras.utils import normalize
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Flatten
from tensorflow.keras.callbacks import TensorBoard
import time

(x_train, y_train), (x_test, y_test) = mnist.load_data()

x_train = normalize(x_train, axis=1)
x_test = normalize(x_test, axis=1)

# The following for-loops would train the network for every combinatin of batchsize, topology and activation function.
# Comment out what you don't need.
for batchsize in range(10, 80, 10):
    for layers in ((64, 64), (128, 128), (256, 256), (128, 64, 32), (30, 20, 10), (28*28, 20)):
        for activation, name in zip((tf.nn.tanh, tf.nn.sigmoid, tf.nn.relu, tf.nn.leaky_relu), ('tanh', 'sigmoid', 'relu', 'lrelu')):
            print('logs/MNIST-batchsize{}-{}{}'.format(batchsize, name, layers))
            board = TensorBoard(log_dir='logs/MNIST-batchsize{}-{}{}'.format(batchsize, name, layers))

            model = Sequential()
            model.add(Flatten()) # First, flatten the 28*28 inputs to a 1-d array.
            for size in layers:
                model.add(Dense(size, activation=activation))
            model.add(Dense(10, activation=tf.nn.softmax))

            model.compile(optimizer='adam',
                        loss='sparse_categorical_crossentropy',
                        metrics=['accuracy'])
            model.fit(x_train, y_train, epochs=6, batch_size=batchsize, validation_data=(x_test, y_test), callbacks=[board])