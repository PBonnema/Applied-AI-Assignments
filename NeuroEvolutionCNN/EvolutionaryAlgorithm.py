from typing import TypeVar, Generic, List, Callable, NewType, Type
from ISelectionStrategy import ISelectionStrategy

TPopMember = TypeVar('TPopMember')
Population = NewType('Population', List[TPopMember])
class EvolutionaryAlgorithm(Generic[TPopMember]):
    """The main evolutionary algorithm.
    
    Use the setters to configure the algorithm. Then call run() to run the algorithm
    and receive back the evolved and sorted population in descending order.

    TPopMember (Generic parameter) -- This is the type of the population members.
        IPopMember is a convenience class that defines the necessary members that TPopMember must implement
        to be able to be used by the evolutionary algorithm. However, it is not mandatory to derive from that class.
    """
    @staticmethod
    def __sortDesc(population: Population) -> None:
        """Sorts the population on fitness in ascending order."""
        population.sort(key=lambda member: member.getFitness(), reverse=True)

    def __init__(self,
            memberCls: Type[TPopMember],
            populationSize: int,
            selectionStrategy: ISelectionStrategy,
            eliteSelecteesCount: int,
            stopCondition: Callable[[int, Population], bool],
            callback: Callable[[int, Population], None]) -> None:
        """
        selectionStrategy -- Set the selection strategy to use. Several standard strategies are already implemented.
        stopCondition -- The the function to determine when to stop. The algorithm will stop if this returns true.
        callback -- Called after each generation. Can be used for any additional work like printing things or tracking statistics.
        """
        super().__init__()
        self.__memberCls = memberCls
        self.__population = Population([])
        self.__populationSize = populationSize
        self.__selectionStrategy = selectionStrategy
        self.__stopCondition = stopCondition
        self.__callback = callback
        self.__eliteSelecteesCount = eliteSelecteesCount
    
    def run(self) -> Population:
        """Runs the algorithm. Call after having configured it using the setters.
        Return -- The evolved population, sorted on fitness in descending order.
        """
        generation = 0
        self.__population = Population([self.__memberCls() for _ in range(self.__populationSize)])
        self.__evaluate()
        EvolutionaryAlgorithm.__sortDesc(self.__population)
        while not self.__stopCondition(generation, self.__population):
            selecteesCount = self.__populationSize - self.__eliteSelecteesCount
            selecteesCount += selecteesCount % 2
            selectees = self.__selectionStrategy.select(self.__population, selecteesCount)
            # Keep only the elites before recombination. recombinate() will append new children to the population after our elites.
            del self.__population[self.__eliteSelecteesCount:]
            self.__recombinate(selectees)
            self.__mutate()
            self.__evaluate()
            EvolutionaryAlgorithm.__sortDesc(self.__population)
            self.__callback(generation, self.__population)
            generation += 1
        return self.__population

    def __evaluate(self) -> None:
        for member in self.__population:
            member.evaluate()

    def __recombinate(self, selectees: Population) -> None:
        """This function extends the population by recombinating the given selectees.
        selectees must contain parents of the new children.
        crossover() is called on each parent as selectees[n*2] with another parent as selectees[n*2+1].
        """
        for parentA, parentB in zip(selectees[0::2], selectees[1::2]):
            self.__population.extend(parentA.crossover(parentB))
        del self.__population[self.__populationSize:]

    def __mutate(self) -> None:
        # Make sure not to mutate the elite members.
        for member in self.__population[self.__eliteSelecteesCount:]:
            member.mutate()