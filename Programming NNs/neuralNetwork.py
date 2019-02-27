from neuron import Neuron
from inputNeuron import InputNeuron
from biasNeuron import BiasNeuron

import numpy as np

class NeuronInfo():
    """Represents the information needed to instantiate a single neuron.
    So this class doesn't model the neuron itself. It is just a helper class to specify the topology of the network.
    """
    def __init__(self, activation_function = None, activation_function_derivative = None, weights = None):
        """
        `activation_function` -- A function that accepts a single number as an argument and returns a single number.
        This is the activation function of the neuron.\n
        `activation_function_derivative` -- a function with the same signature as `activation_function` that also returns a single number.
        It is the derivative of the activation function which is used during the training phase.\n
        `weights` -- An array of numbers containing the weights of the connections leading to this neuron.
        The last element is the weight for the connection with the bias in the previous layer of the neuron.
        """
        self.activation_function = activation_function
        self.activation_function_derivative = activation_function_derivative
        self.weights = weights

class NeuralNetwork:
    """Represents a feed-forward neural network that supports a training phase using the back-propagation algorithm.
    `activate()` will run a set of input values through the network and return an array of output values.
    `train()` will train the network given a set of inputs and corresponding expected responses.
    
    The topology of the network is fixed and specified as parameters to `__init__()`.
    Any number of input and output neurons and any number of hidden layers of any size are supported.
    """
    @staticmethod
    def __activate_layer(layer):
        for neuron in layer:
            neuron.activate()

    @staticmethod
    def __randomize_layer(layer, rng, min, max):
        for neuron in layer:
            neuron.randomize_weights(rng, min, max)

    @staticmethod
    def __update_layer(layer, learning_rate):
        for neuron in layer:
            neuron.update(learning_rate)

    def __init__(self, input_count, hidden_neurons, output_neurons):
        """Creates and initializes the neural network.
        
        `input_count` -- The number of inputs for this network.
        `hidden_neurons` -- A 2-d list of `NeuronInfo` instances. Each row represents a hidden layer.
        The layers can be of different lenghts.
        `output_neurons` -- a 1-d list of `NeuronInfo` instances. This represents output neurons.
        """
        # Initialize the input layer. The last element of the all layers will be the bias neuron.
        self.__input_layer = [InputNeuron() for i in range(input_count)] + [BiasNeuron()]

        # Initialize the hidden layers.
        self.__hidden_layers = []
        last_layer = self.__input_layer
        for infos in hidden_neurons:
            # The last element of the all layers will be the bias neuron.
            layer = [Neuron(last_layer, info.weights, info.activation_function, info.activation_function_derivative) for info in infos] + [BiasNeuron()]
            last_layer = layer
            self.__hidden_layers.append(layer)

        # Initialize the output layer.
        self.__output_layer = [Neuron(last_layer, info.weights, info.activation_function, info.activation_function_derivative) for info in output_neurons]

    def __repr__(self):
        return 'input:\n{}\nhidden:\n{}\noutput:\n{}'.format(self.__input_layer, '\n'.join(repr(l) for l in self.__hidden_layers), self.__output_layer)

    def randomize(self, rng, min, max):
        """Randomizes all the weights in the network to be in the interval [`min`, `max`).
        
        `rng` - A `numpy.random.RandomState` instance.
        `min` - The lower bound of the interval that the weigts can be in.
        `max` - The upper bound of the interval that the weigts can be in.
        """
        for hidden_layer in self.__hidden_layers:
            NeuralNetwork.__randomize_layer(hidden_layer, rng, min, max)
        NeuralNetwork.__randomize_layer(self.__output_layer, rng, min, max)

    def activate(self, inputs):
        """Activates the network by inputting all `inputs` to the input layer, activating all layers 1 by 1
        and finally returns the outputs as a 1-d array of numbers.
        """
        # Input the values into the network.
        for input_neuron, input in zip(self.__input_layer, inputs):
            input_neuron.set_output(input)

        # Active the network.
        for hidden_layer in self.__hidden_layers:
            NeuralNetwork.__activate_layer(hidden_layer)

        NeuralNetwork.__activate_layer(self.__output_layer)

        # Sample the output.
        output = np.empty(len(self.__output_layer))
        for i, output_neuron in enumerate(self.__output_layer):
            output[i] = output_neuron.get_output()
        return output

    def train(self, inputs, desired_outputs, learning_rate, max_epochs):
        """Trains the network using the back-propagation algorithm.
        
        `inputs` -- A 2-d array of numbers containing a set of input values to train with.
        `desired_outputs` --  A 2-d array of numbers containing the expected responses.
        `learning_rate` -- The learning rate. A ratio of the weight delta that is calculated for each weight.
        Set higher to learn faster and to escape local minima. Set lower to prevent overshooting.
        `max_loops` -- The number of training cycles that will be performed. A training cycle consists of
        doing back-propagation once for each row of `inputs`. The algorithm stops before this point if the error reaches `0`.
        """
        error = float('inf')
        errors = np.empty(max_epochs)
        for epoch in range(max_epochs):
            error = 0
            for input, desired_output in zip(inputs, desired_outputs):
                # Run the network with the input to determine its actual output
                # This call also makes sure that the interal sum values of the neurons are up to date for this particular input sample.
                actual_output = self.activate(input)
                error += np.sum((desired_output - actual_output) ** 2)

                # Now compare the output to the expected response and update the network
                for neuron, actual, desired in zip(self.__output_layer, actual_output, desired_output):
                    # The cost of the output neurons is (actual - desired) because this is the derivative of our cost function w.r.t the activation value of our output neurons
                    # since the cost function itself is (1/2 * (desired - actual) ^ 2).
                    neuron.add_cost(actual - desired)
                    neuron.update(learning_rate)
                    
                for layer in reversed(self.__hidden_layers):
                    NeuralNetwork.__update_layer(layer, learning_rate)

            errors[epoch] = error
            if error == 0:
                break

        return errors