from neuron import Neuron

class BiasNeuron(Neuron):
    def __init__(self):
        super().__init__(None, None, None, None)
        self._output = 1

    def __repr__(self):
        return 'bias'

    def activate(self):
        pass

    def update(self, desired_activation):
        pass