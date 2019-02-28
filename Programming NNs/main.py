from neuralNetwork import NeuralNetwork, NeuronInfo

import math
import numpy as np
import matplotlib.pyplot as plt
from functools import partial

# Possible activation functions
def threshold(threshold, sum):
    return 1.0 if sum >= threshold else 0.0

def ReLU(sum):
    return max(0.0, sum)

def ReLU_derivative(sum):
    return 1 if sum >= 0 else 0

def LReLU(sum):
    return max(0.01 * sum, sum)

def LReLU_derivative(sum):
    return 1 if sum >= 0 else 0.01

def sigmoid(sum):
    return 1 / (1 + math.exp(-sum))

def sigmoid_derivative(sum):
    sig = sigmoid(sum)
    return sig * (1 - sig)

def tanh_derivative(sum):
    return 1 - math.tanh(math.tanh(sum))

np.set_printoptions(precision=4)

# 4.3 A. NOR Gate
nn = NeuralNetwork(
    3,
    [],
    [
        NeuronInfo(partial(threshold, 0), weights = [-1, -1, -1, 0])
    ]
)
print('NOR Gate:\n{}\n'.format(nn))

print('[0, 0, 0] -> {}'.format(nn.activate([0, 0, 0])))
print('[0, 0, 1] -> {}'.format(nn.activate([0, 0, 1])))
print('[0, 1, 0] -> {}'.format(nn.activate([0, 1, 0])))
print('[0, 1, 1] -> {}'.format(nn.activate([0, 1, 1])))
print('[1, 0, 0] -> {}'.format(nn.activate([1, 0, 0])))
print('[1, 0, 1] -> {}'.format(nn.activate([1, 0, 1])))
print('[1, 1, 0] -> {}'.format(nn.activate([1, 1, 0])))
print('[1, 1, 1] -> {}'.format(nn.activate([1, 1, 1])))
print()

# 4.3 A. Adder
nn = NeuralNetwork(
    2,
    [
        [
            NeuronInfo(partial(threshold, 0.5), weights = [0.5, 0.5, 0]),
            NeuronInfo(partial(threshold, 1.0), weights = [0.5, 0.5, 0]),
        ]
    ],
    [
        NeuronInfo(partial(threshold, 1), weights = [0, 1, 0]), # Carry
        NeuronInfo(partial(threshold, 1), weights = [1, -1, 0]), # Sum
    ]
)
print('Adder:\n{}\n'.format(nn))

print('[0, 0] -> {}'.format(nn.activate([0, 0])))
print('[0, 1] -> {}'.format(nn.activate([0, 1])))
print('[1, 0] -> {}'.format(nn.activate([1, 0])))
print('[1, 1] -> {}'.format(nn.activate([1, 1])))
print()

# 4.3 B. Delta Rule
# Neural Network Settings
learning_rate = 100
epochs = 1000
weight_lower_bound = -1
weight_upper_bound = 1
rng = np.random.RandomState(seed = None)
nn = NeuralNetwork(
    3,
    [],
    [
        NeuronInfo(sigmoid, sigmoid_derivative)
    ]
)

print('Training a NOR Gate:\n{}\n'.format(nn))
nn.randomize(rng, weight_lower_bound, weight_upper_bound)
print('Training...')
inputs = np.array([
    [0, 0, 0],
    [0, 0, 1],
    [0, 1, 0],
    [0, 1, 1],
    [1, 0, 0],
    [1, 0, 1],
    [1, 1, 0],
    [1, 1, 1],
])

expected_outputs = np.array([
    [1],
    [0],
    [0],
    [0],
    [0],
    [0],
    [0],
    [0],
])

# This learning rate is extremely high, yet yields very good results.
# I think this is due to the fact that the perfect output values are 0 and 1 and the sigmoid function is only 0 or 1 at its extremes
# and needs an infinite input for that. To do this the network is trying to train infinite weights.
nn.train(inputs, expected_outputs, learning_rate, epochs)

print('After training:')
MSE = 0.0
for input, expected_output in zip(inputs, expected_outputs):
    output = nn.activate(input)
    print('{} -> {}'.format(input, output))
    MSE += ((expected_output - output) ** 2).sum()
print('MSE: {}\n'.format(MSE / len(inputs)))

# 4.3 C. XOR and Back-propagation
# Neural Network Settings
learning_rate = 20
epochs = 2000
weight_lower_bound = -1
weight_upper_bound = 1
rng = np.random.RandomState(seed = None)
nn = NeuralNetwork(
    2,
    [
        [
            NeuronInfo(sigmoid, sigmoid_derivative),
            NeuronInfo(sigmoid, sigmoid_derivative)
        ],
    ],
    [
        NeuronInfo(sigmoid, sigmoid_derivative)
    ]
)

print('Training a XOR Gate:\n{}\n'.format(nn))
nn.randomize(rng, weight_lower_bound, weight_upper_bound)
print('Training...')
inputs = np.array([
    [0, 0],
    [0, 1],
    [1, 0],
    [1, 1],
])

expected_outputs = np.array([
    [0],
    [1],
    [1],
    [0],
])

nn.train(inputs, expected_outputs, learning_rate, epochs)

print('After training:')
MSE = 0.0
for input, expected_output in zip(inputs, expected_outputs):
    output = nn.activate(input)
    print('{} -> {}'.format(input, output))
    MSE += ((expected_output - output) ** 2).sum()
print('MSE: {}\n'.format(MSE / len(inputs)))

# 4.3 D. Iris dataset
# Neural Network Settings
validation_percentage = 0.6
minimum_certainty = 0.8
learning_rate = 0.12
epochs = 500
weight_lower_bound = -1
weight_upper_bound = 1
rng = np.random.RandomState(seed = 0)
nn = NeuralNetwork(
    4,
    [
        [
            NeuronInfo(sigmoid, sigmoid_derivative),
            NeuronInfo(sigmoid, sigmoid_derivative),
            NeuronInfo(sigmoid, sigmoid_derivative),
            NeuronInfo(sigmoid, sigmoid_derivative),
        ],
    ],
    [
        NeuronInfo(sigmoid, sigmoid_derivative),
        NeuronInfo(sigmoid, sigmoid_derivative),
        NeuronInfo(sigmoid, sigmoid_derivative),
    ]
)

# Read in dataset
data_points = np.genfromtxt('iris.csv', delimiter = ',', usecols = [0, 1, 2, 3])
labels = np.genfromtxt('iris.csv', delimiter = ',', usecols = [4], dtype = str)

unique_labels = np.unique(labels)
label_codes = np.empty((labels.shape[0], unique_labels.shape[0]))
for i, label in enumerate(labels):
    label_codes[i] = unique_labels == label

# Split the data and labels into a training set and a validation set
validation_count = int(data_points.shape[0] * validation_percentage)
indices = rng.permutation(data_points.shape[0])
training_idx, val_idx = indices[:data_points.shape[0] - validation_count], indices[validation_count:]
training_samples, val_samples = data_points[training_idx,:], data_points[val_idx,:]
training_labels, val_labels = label_codes[training_idx,:], label_codes[val_idx,:]

print('Training Iris classifier:\n{}\n'.format(nn))
nn.randomize(rng, weight_lower_bound, weight_upper_bound)
print('Training...')
errors = nn.train(training_samples, training_labels, learning_rate, epochs)

print('After training:')

def threshold_output(output, min, max, minimum_certainty):
    assert minimum_certainty >= 0.5
    assert max > min
    copy = output.copy()
    copy[np.nonzero(output >= (max - min) * minimum_certainty + min)] = max
    copy[np.nonzero(output <= max - (max - min) * minimum_certainty)] = min
    return copy

# Show the result per input sample
MSE = 0.0
correct_count = 0
for input, expected_output in zip(val_samples, val_labels):
    output = nn.activate(input)
    correct = (threshold_output(output, 0.0, 1.0, minimum_certainty) == expected_output).all()
    correct_count += correct
    print('{} -> {} expected: {}. {}'.format(input, output, expected_output, 'Correct' if correct else 'Not correct' ))

    MSE += ((expected_output - output) ** 2).sum()

# Show the aggregate results
print('Mean squared error for validation set: {:0.5f}\nCorrect: {:0.2f}% ({}/{})'.format( \
    MSE / val_samples.shape[0], \
    100 * correct_count / val_samples.shape[0], \
    correct_count, \
    val_samples.shape[0]))

plt.plot(errors / val_samples.shape[0], linewidth = 1.0)
plt.xlabel('Epoch')
plt.ylabel('Mean Squared Error')
plt.ylim(bottom = 0, top = 0.7)
plt.show()