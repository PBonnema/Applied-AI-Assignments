import random

class Neuron:
    def __init__(self, prev_layer, weights, activation_function, activation_function_derivative):
        self._sum = 0.0
        self._output = 0.0
        self._prev_layer = prev_layer
        self._weights = weights
        self._activation_function = activation_function
        self._activation_function_derivative = activation_function_derivative

    def __repr__(self):
        return 'neuron'

    def randomize_weights(self):
        self._weights = [random.uniform(-1, 1) for _ in range(len(self._prev_layer))]

    def activate(self):
        self._sum = 0.0
        for neuron, weight in zip(self._prev_layer, self._weights):
            self._sum += neuron.get_output() * weight
        self._output = self._activation_function(self._sum)

    def get_output(self):
        return self._output

    def update(self, desired_activation, learning_rate):
        # The Delta rule
        error = desired_activation - self._output
        if error != 0:
            constant = learning_rate * self._activation_function_derivative(self._sum) * error
            for i, neuron in enumerate(self._prev_layer):
                self._weights[i] += neuron.get_output() * constant
        return error