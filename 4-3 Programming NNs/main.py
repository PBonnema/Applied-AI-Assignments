from neuralNetwork import NeuralNetwork, NeuronInfo

import math
import numpy as np
from functools import partial

def threshold(threshold, sum):
    return 1.0 if sum >= threshold else 0.0

def ReLU(sum):
    return max(0.0, sum)

def ReLU_derivative(sum):
    return 1 if sum > 0 else 0

def sigmoid(sum):
    return 1 / (1 + math.exp(-sum))

def sigmoid_derivative(sum):
    sig = sigmoid(sum)
    return sig * (1 - sig)

def tanh_derivative(sum):
    return 1 - math.tanh(math.tanh(sum))

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
nn = NeuralNetwork(
    3,
    [],
    [
        NeuronInfo(sigmoid, sigmoid_derivative)
    ]
)
print('Training a NOR Gate:\n{}\n'.format(nn))
nn.randomize()
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
nn.train(inputs, expected_outputs, 1, 10000)

print('After training:')
MSE = 0.0
for input, expected_output in zip(inputs, expected_outputs):
    output = nn.activate(input)
    print('{} -> {}'.format(input, output))
    MSE += np.sum((expected_output - output) ** 2)
print('MSE: {}'.format(MSE))