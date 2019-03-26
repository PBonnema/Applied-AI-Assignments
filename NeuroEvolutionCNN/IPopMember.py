from __future__ import annotations # Necessary to allow class members to typehint using their parent class. Supported from Python 3.7 onwards (https://stackoverflow.com/questions/40049016/using-the-class-as-a-type-hint-for-arguments-in-its-methods)
import abc
from io import TextIOBase

class IPopMember(metaclass=abc.ABCMeta):
    """A convenience interface class that defines the necessary members that PopMember classes must implement
    to be able to be used by the evolutionary algorithm. However, it is not mandatory to derive from this class.
    """

    def __init__(self) -> None:
        self.__fitness = 0.0
    
    def evaluate(self) -> None:
	    self.__fitness = self.evaluateFitness()

    @abc.abstractmethod
    def evaluateFitness(self) -> float:
        pass

    @abc.abstractmethod
    def crossover(self, otherParent: IPopMember) -> IPopMember:
        pass

    @abc.abstractmethod
    def mutate(self) -> None:
        pass

    def getFitness(self) -> float:
        """Will simply return the cached fitness. Call evaluate() to recalculate the fitness."""
        return self.__fitness