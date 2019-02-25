from neuron import Neuron

class InputNeuron(Neuron):
    def __init__(self):
        super().__init__(None, None, None, None)

    def __repr__(self):
        return 'input'

    def activate(self):
        pass

    def update(self, desired_activation):
        pass

    def set_output(self, value):
        self._output = value