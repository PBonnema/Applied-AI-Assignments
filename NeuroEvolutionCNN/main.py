#import tensorflow as tf
# from tensorflow.keras.utils import normalize
# from tensorflow.keras.models import Sequential
# from tensorflow.keras.layers import Dense, Conv2, Flatten
# from tensorflow.keras.callbacks import TensorBoard
import time
import sys

from EvolutionaryAlgorithm import EvolutionaryAlgorithm
from SelectionTournament import SelectionTournament
from TestPopMember import TestPopMember

strategy = SelectionTournament(14, 1)
ea = EvolutionaryAlgorithm(
    memberCls=TestPopMember,
    populationSize=200,
    selectionStrategy=strategy,
    eliteSelecteesCount=1,
    stopCondition=lambda gen, pop: gen > 50,
    callback=lambda gen, pop: print(pop[0])
)

ea.run()