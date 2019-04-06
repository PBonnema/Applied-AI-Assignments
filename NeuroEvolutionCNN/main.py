import time
import sys

from EvolutionaryAlgorithm import EvolutionaryAlgorithm
from SelectionTournament import SelectionTournament
from TestPopMember import TestPopMember

runnningTotalGens = 0

def stopCondition(gen: int, pop: list):
    global runnningTotalGens
    if (pop[0].getFitness() == 98938.0):
        runnningTotalGens += gen
        return True
    return False

strategy = SelectionTournament(14, 1.0)
ea = EvolutionaryAlgorithm(
    memberCls=TestPopMember,
    populationSize=200,
    selectionStrategy=strategy,
    eliteSelecteesCount=2,
    stopCondition=stopCondition,
    callback=lambda gen, pop: None
)

for run in range(1, 11):
    ea.run()
    print(runnningTotalGens / run)

# print(runnningTotalGens / 200)