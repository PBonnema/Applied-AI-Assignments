from importData import import_training_data, import_validation_data, import_unlabeled
from KNN import KNN

import json
import numpy as np
import itertools

# Import the training data, validation data, and the unlabeled data
training_labels, training_samples = import_training_data('..\\dataset1.csv')
validation_labels, validation_samples = import_validation_data('..\\validation1.csv')
unlabeled_samples = import_unlabeled('..\\days.csv')

# Oddly, KNN.NeighbourWeightingStrategy.WEIGHTED_MAJORITY_VOTE seems to degrade accuracy a bit
options = KNN.KNNOptions(max_K = 65, P = 2, weights = None, neighbour_weighting_strategy = KNN.NeighbourWeightingStrategy.MAJORITY_VOTE)
predictor = KNN(options, training_labels, training_samples)
predictor.determine_best_K(validation_labels, validation_samples)
labels = predictor.predict(unlabeled_samples)

for label, sample in zip(labels, unlabeled_samples):
    print('{} should be {}'.format(sample, label))