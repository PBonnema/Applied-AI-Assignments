from biasNeuron import BiasNeuron

class InputNeuron(BiasNeuron):
    """Represents an input neuron. It exposes only a public `set_output()`."""
    def __init__(self):
        super().__init__()

    def __repr__(self):
        return 'input'

    def set_output(self, value):
        self._set_output(value)