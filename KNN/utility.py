import numpy as np
from collections import Counter, defaultdict

def minkowski_squared(sample1, sample2, weights = None, P = 2):
    if weights is not None:
        return ((np.fabs(sample1 - sample2) * weights) ** P).sum()
    return (np.fabs(sample1 - sample2) ** P).sum()

def majority_vote(neighbour_labels, _ = None):
    return Counter(neighbour_labels).most_common(1)[0][0]

def weighted_majority_vote(neighbour_labels, neighbour_distances_squared):
    label_votes = defaultdict(int)
    neighbour_distances_squared = 1.0 / (neighbour_distances_squared ** 0.5)
    for i in range(neighbour_labels.shape[0]):
        label_votes[neighbour_labels[i]] += neighbour_distances_squared[i]

    return max(label_votes, key = label_votes.get)