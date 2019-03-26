from typing import TypeVar, Generic, List, Callable, NewType
import abc

TPopMember = TypeVar('TPopMember')
Population = NewType('Population', List[TPopMember])
class ISelectionStrategy(Generic[TPopMember], metaclass=abc.ABCMeta):
    @abc.abstractmethod
    def select(self, population: Population, membersToSelect: int) -> Population:
        """Must return membersToSelect selectees from the population. These will act as parents during recombination.
        The returned vector may contain the same selectee more than once."""
        pass