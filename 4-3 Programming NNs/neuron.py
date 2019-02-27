class Neuron():
    """This class represents a neuron with backpropagation functionality. It is designed to work in feed-forward networks.

    It retains a list of references to neurons in the previous layer.
    When the neuron activates, it asks for the activation values of the neurons in its list and stores its
    own activation value. This value can be queried with `get_output()`.

    `update()` implements the backpropagation learning algorithm. The neuron propagates its cost back to
    the neurons in its list by calling `add_cost()` on them.
    """
    def __init__(self, prev_layer, weights, activation_function, activation_function_derivative):
        self.__sum = 0.0
        self.__output = 0.0
        self.__cost = 0.0
        self.__prev_layer = prev_layer
        self.__weights = weights
        self.__activation_function = activation_function
        self.__activation_function_derivative = activation_function_derivative

    def __repr__(self):
        return 'neuron'

    def randomize_weights(self, rng, min, max):
        """Randomizes all weights to values in the interval [`min`, `max`).
        
        `rng` - A `numpy.random.RandomState` instance.
        `min` - The lower bound of the interval that the weigts can be in.
        `max` - The upper bound of the interval that the weigts can be in.
        """
        self.__weights = rng.rand(len(self.__prev_layer)) * (max - min) + min

    def activate(self):
        """Activates the neuron. It asks all neurons in the previous layer for their input and calculates the output."""
        self.__sum = 0.0
        for neuron, weight in zip(self.__prev_layer, self.__weights):
            self.__sum += neuron.get_output() * weight
        self.__output = self.__activation_function(self.__sum)

    def update(self, learning_rate):
        """Updates the weights leading to this neuron based on back-propagation using the cost that was previously set by `add_cost()`.
        `activate()` needs to be called before calling `update()` because an internal sum value will be used that
        is determined by `activate()`. Otherwise, an outdated value for the input sum will be used.
        
        This method also propagates its total cost (mulitplied by the weigts) to the neurons in the previous layer
        and resets the internal cost value to 0.0.

        `learning_rate` -- The learning rate. A ratio of the calculated weight delta that is used to update weights.
        """
        # The Delta rule.
        self.__cost *= self.__activation_function_derivative(self.__sum)
        delta = learning_rate * self.__cost
        for i, neuron in enumerate(self.__prev_layer):
            # Propagate the cost to the previous layer using the old weight.
            neuron.add_cost(self.__cost * self.__weights[i])

            # Delta represents the derivative of the weight w.r.t. to the cost (it is proportional to it) of this neuron
            # so to lower our cost we need to subtract it from our current weight.
            self.__weights[i] -= neuron.get_output() * delta

        # We've used the cumulative cost value so we can reset it and start afresh.
        self.__cost = 0.0

    def add_cost(self, cost):
        """Call this function to accumulate the cost for this neuron.
        The accumulated cost value will be used by `update()`."""
        self.__cost += cost

    def get_output(self):
        return self.__output

    def _set_output(self, value):
        self.__output = value
