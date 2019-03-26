from typing import TypeVar, Generic, List, Callable, NewType
import numpy as np
from random import randrange, random

from ISelectionStrategy import ISelectionStrategy#, Population

TPopMember = TypeVar('TPopMember')
Population = NewType('Population', List[TPopMember])
class SelectionTournament(ISelectionStrategy[TPopMember]):
    """Implements the tournament selection algorithm. The selection pressure can be tuned with the tournament size and the p variable.
    p is the chance that the winner is selected. If it's not selected, then the 2nd place as a p chance to be selected etc.
    """

    def __init__(self, tournamentSize: int, p: float) -> None:
        super().__init__()
        self.__tournamentSize, self.__p = tournamentSize, p

    def select(self, population: Population, membersToSelect: int) -> Population:
        selectees = Population([])
        popSize = len(population)
        indices = np.full(self.__tournamentSize, -1, dtype = np.int)
        for _ in range(membersToSelect):
            # Pick contenders at random and store their index.
            for index in np.nditer(indices, op_flags=['writeonly']):
                randomIndex = randrange(popSize - 1)
                # Keep picking a new index if the current index has already been choosen.
                while randomIndex in indices:
                    randomIndex = randrange(popSize - 1)
                index[...] = randomIndex

            # Now calculate which place in the tournament result (1th place, 2nd place etc) will be selected.
            winningIndex = self.__calcWinningIndex()
            # And figure out the index of the selectee.
            indices.partition(winningIndex)
            selectees.append(population[indices[winningIndex]])
        return selectees

    def __calcWinningIndex(self) -> int:
        """Calculates which place in the tournament result (1th place, 2nd place etc) will be selected."""
        number = random()
        remainingFraction = 1.0 - self.__p

        index = 0
        while remainingFraction > number and index < self.__tournamentSize - 1:
            index += 1
            remainingFraction *= 1.0 - self.__p

        return index