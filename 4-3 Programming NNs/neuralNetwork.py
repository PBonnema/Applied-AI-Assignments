from neuron import Neuron
from inputNeuron import InputNeuron
from biasNeuron import BiasNeuron

class NeuronInfo():
    def __init__(self, activation_function = None, activation_function_derivative = None, weights = None):
        self.weights = weights
        self.activation_function = activation_function
        self.activation_function_derivative = activation_function_derivative

class NeuralNetwork:
    @staticmethod
    def __activate_layer(layer):
        for neuron in layer:
            neuron.activate()

    @staticmethod
    def __randomize_layer(layer):
        for neuron in layer:
            neuron.randomize_weights()

    def __init__(self, input_count, hidden_neurons, output_neurons):
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
        return '{}\n{}\n{}'.format(repr(self.__input_layer), repr(self.__hidden_layers), repr(self.__output_layer))

    def randomize(self):
        for hidden_layer in self.__hidden_layers:
            NeuralNetwork.__randomize_layer(hidden_layer)
        NeuralNetwork.__randomize_layer(self.__output_layer)

    def activate(self, inputs):
        for input_neuron, i in zip(self.__input_layer, inputs):
            input_neuron.set_output(i)

        for hidden_layer in self.__hidden_layers:
            NeuralNetwork.__activate_layer(hidden_layer)

        NeuralNetwork.__activate_layer(self.__output_layer)

        output = []
        for output_neuron in self.__output_layer:
            output.append(output_neuron.get_output())
        return output

    def train(self, inputs, desired_outputs, rate, max_loops):
        error = float('inf')
        for _ in range(max_loops):
            error = 0
            for input, desired_output in zip(inputs, desired_outputs):
                self.activate(input)
                for neuron, desired in zip(self.__output_layer, desired_output):
                    neuron.update(desired, rate)

                actual_outputs = self.activate(input)
                for actual, desired in zip(actual_outputs, desired_output):
                    error += (actual - desired) ** 2
            if error == 0:
                break