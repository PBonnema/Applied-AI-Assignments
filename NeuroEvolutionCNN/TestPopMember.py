from __future__ import annotations # Necessary to allow class members to typehint using their parent class. Supported from Python 3.7 onwards (https://stackoverflow.com/questions/40049016/using-the-class-as-a-type-hint-for-arguments-in-its-methods)
from IPopMember import IPopMember
from random import randrange
from io import TextIOBase
from typing import List
import numpy as np

class TestPopMember(IPopMember):
    def __init__(self) -> None:
        super().__init__()
        self.A = randrange(64)
        self.B = randrange(64)
        self.C = randrange(64)
        self.D = randrange(64)

    def evaluateFitness(self) -> float:
        return (self.A - self.B) * (self.A - self.B) \
            + (self.C + self.D) * (self.C + self.D) \
            - (self.A - 30.0) * (self.A - 30.0) * (self.A - 30.0) \
            - (self.C - 40.0) * (self.C - 40.0) * (self.C - 40.0)

    def crossover(self, otherParent: TestPopMember) -> List[TestPopMember]:
        assert(type(otherParent) is TestPopMember)
        child1 = TestPopMember()
        child1.A = self.A
        child1.B = otherParent.B
        child1.C = self.C
        child1.D = otherParent.D
        
        child2 = TestPopMember()
        child2.A = otherParent.A
        child2.B = self.B
        child2.C = otherParent.C
        child2.D = self.D
        return [child1, child2]

    def __flipBit(self, index: int) -> None:
        if index < 6:
            self.A ^= 1 << index
        elif index < 12:
            self.B ^= 1 << index - 6
        elif index < 18:
            self.C ^= 1 << index - 12
        else:
            self.D ^= 1 << index - 18

    def mutate(self) -> None:
        flipCount = np.random.poisson((0.8 / 24) * 24)
        indices = []
        for _ in range(flipCount):
            randomIndex = randrange(24)
            # Keep picking a new index if the current index has already been chosen.
            while randomIndex in indices:
                randomIndex = randrange(24)
            indices.append(randomIndex)
            self.__flipBit(randomIndex)

    def __repr__(self) -> str:
        return f'{(self.A, self.B, self.C, self.D)}, {self.getFitness()}'