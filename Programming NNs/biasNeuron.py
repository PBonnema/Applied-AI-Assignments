from neuron import Neuron

class BiasNeuron(Neuron):
    """Represents a bias. The bias is modelled as a separate neuron to simplify the algorithm.
    The output value of this neuron is hardcoded to always be `1` and
    all of `Neuron`s methods are overriden to be a no-op yet still be callable. This is done to simplify the algorithms.
    """
    def __init__(self):
        super().__init__(None, None, None, None)
        self._set_output(1)

    def __repr__(self):
        return 'bias'

    def randomize_weights(self, rng, min, max):
        pass

    def activate(self):
        pass

    def update(self, learning_rate):
        pass

    def add_cost(self, cost):
        pass