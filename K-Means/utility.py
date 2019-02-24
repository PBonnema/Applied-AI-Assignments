import numpy as np
from collections import Counter, defaultdict

def euclidean_squared(sample1, sample2, weights = None):
    if weights is not None:
        return (((sample1 - sample2) * weights) ** 2).sum()
    return ((sample1 - sample2) ** 2).sum()

def minkowski_squared(sample1, sample2, weights = None, P = 2):
    if weights is not None:
        return ((np.fabs(sample1 - sample2) * weights) ** P).sum()
    return (np.fabs(sample1 - sample2) ** P).sum()

def majority_vote(neighbour_labels, _ = None):
    """Returns the label that is most common in `neighbour_labels`."""
    return Counter(neighbour_labels).most_common(1)[0][0]

def weighted_majority_vote(neighbour_labels, neighbour_distances_squared):
    """Returns the label from `neighbour_labels` with the most amount of votes where the vote of each neighbour equals 1 / neighbour distance.
    
    This is a weighted (by 1/distance) version of `majority_vote()`"""
    label_votes = defaultdict(int)
    neighbour_votes = 1.0 / (neighbour_distances_squared ** 0.5)
    for i in range(neighbour_labels.shape[0]):
        label_votes[neighbour_labels[i]] += neighbour_votes[i]

    return max(label_votes, key = label_votes.get)