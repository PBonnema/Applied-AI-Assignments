from IPopMember import IPopMember
from random import randrange
from io import TextIOBase
import numpy as np

class TestPopMember(IPopMember):
    def __init__(self) -> None:
        super().__init__()
        self.A = randrange(64)
        self.B = randrange(64)
        self.C = randrange(64)
        self.D = randrange(64)

    def evaluateFitness(self) -> float:
        return (self.A - self.B) * (self.A - self.B) + (self.C + self.D) * (self.C + self.D) - (self.A - 30.0) * (self.A - 30.0) * (self.A - 30.0) - (self.C - 40.0) * (self.C - 40.0) * (self.C - 40.0)

    def crossover(self, otherParent: IPopMember) -> IPopMember:
        assert(type(otherParent) is TestPopMember)
        child = TestPopMember()
        child.A = self.A
        child.B = otherParent.B
        child.C = self.C
        child.D = otherParent.D
        return child

    def mutate(self) -> None:
        np.random.poisson(0.8)

        mask = 1 << randrange(6)
        self.A ^= mask
        mask = 1 << randrange(6)
        self.B ^= mask
        mask = 1 << randrange(6)
        self.C ^= mask
        mask = 1 << randrange(6)
        self.D ^= mask

    def __repr__(self) -> str:
        return f'{(self.A, self.B, self.C, self.D)}, {self.getFitness()}'