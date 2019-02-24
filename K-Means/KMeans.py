import utility as util

import sklearn.cluster as skc
import numpy as np
from collections import Counter, defaultdict
from enum import Enum
import sys
import matplotlib.pyplot as plt

class KMeans:
    """Implements the K-Means algorithm."""

    class KMeansOptions:
        """Configures the KNN algorithm."""

        def __init__(self, max_K, sensitivity, repeat, seed):
            """
            `max_K` -- The maximum value for K that `determine_best_K()` will try.\n
            `sensitivity` -- The algorithm will declare convergence if, after an iteration, the largest movement of all centroids is <= `sensitivity`\n
            `repeat` -- This is the number of times that the intra cluster distance will be measured for each K when deciding the best value for K.
                This is necessary since the initialization of centroids in random which might affect the outcome.\n
            `seed` -- The seed to use for the random number generatior which is used for deciding on the start locations for the centroids.
            """
            self.max_K, self.sensitivity, self.repeat, self.seed = max_K, sensitivity, repeat, seed

    @staticmethod
    def __get_closest_centroid_index_and_distance(sample, centroids):
        """Returns the index into `centroids` of the centroid that is the closest to `sample` and the squared distance to that centroid.
        
        `sample` -- A sample. Must be an ndarray.\n
        `centroids` -- A collection of centroids. Must be an ndarray with shape `[sample.shape[0]] + sample.shape` (i.e. it must be an array of arrays that have the same shape as `sample`.)
        """
        closest_index = sys.maxsize
        lowest_distance_squared = float('inf')
        for i in range(centroids.shape[0]):
            distance_squared = util.euclidean_squared(sample, centroids[i])
            if distance_squared < lowest_distance_squared:
                lowest_distance_squared, closest_index = distance_squared, i

        return closest_index, lowest_distance_squared

    @staticmethod
    def __calculate_centroids(samples, centroids):
        """Given a set of clusters defined by `centroids`, recalculates the actual centroids of those clusters by assigning each sample
        to its closest centroid and then averages all points of the samples per cluster except for that the final division is skipped.
        In other words, the new centroids act as running totals while adding samples to it.
        
        `samples` -- An ndarray of samples. Must be an ndarray.\n
        `centroids` -- The old centroids. Must be an ndarray with shape `[samples[0].shape[0]] + samples[0].shape` (i.e. it must be an array of arrays that have the same shape as the `samples`.)
        
        The first return value is an 1-d ndarray with the sample count for each cluster.
        The second return value is an ndarray with the new centroids with the same shape as `centroids`."""
        new_centroid_counts = np.zeros(centroids.shape[0], dtype = np.int)
        new_centroids = np.zeros(centroids.shape, dtype = centroids.dtype)

        for sample in samples:
            # Add the sample to the choosen new centroid (new centroids act as running totals).
            centroid_index, _ = KMeans.__get_closest_centroid_index_and_distance(sample, centroids)
            new_centroids[centroid_index] += sample
            new_centroid_counts[centroid_index] += 1
        
        return new_centroid_counts, new_centroids

    @staticmethod
    def __calculate_centroid_movement(centroids, new_centroids):
        """Returns a 1-d ndarray with the movement of `centroids` given the new positions of the centroids in `new_centroids`.
        The new centroid for `centroids[i]` is assumed to be `new_centroids[i]`.
        
        `centroids` -- The old centroids. Must be an ndarray.\n
        `new_centroids` -- The new centroids. Must be an ndarray.
        
        The returned ndarray contains the squared distances."""
        centroid_movement = np.empty(centroids.shape[0])
        for movement, centroid, new_centroid in zip(np.nditer(centroid_movement, op_flags=['writeonly']), centroids, new_centroids):
            movement[...] = util.euclidean_squared(centroid, new_centroid)

        return centroid_movement

    @staticmethod
    def __reset_empty_clusters(samples, empty_cluster_indices, centroids, rng):
        """Replaces each empty cluster (defined by `centroids`) with a random sample from `samples`.
        Also makes sure that centroids are not placed on the same sample.
        
        `samples` -- An ndarray of samples to choose random samples from in case a centroid is empty. Must be an ndarray.\n
        `empty_cluster_indices` -- Must be an ndarray containing indices of the centroids that are empty (isn't the closest centroid for any of the `samples`).\n
        `centroids` -- The centroids. Must be an ndarray with shape `[samples[0].shape[0]] + samples[0].shape` (i.e. it must be an array of arrays that have the same shape as the `samples`.)\n
        `rng` -- A NumPy RandomState instance.
        """
        random_samples = samples[rng.choice(samples.shape[0], empty_cluster_indices.shape[0], replace = False)]
        centroids[empty_cluster_indices] = random_samples

    @staticmethod
    def __calculate_intra_distance(samples, centroids):
        """Returns the total within-cluster distance, also known as the intra cluster distance, as the sum of squares of distances.
        
        `samples` -- An ndarray of samples. Must be an ndarray.\n
        `centroids` -- The centroids. Must be an ndarray with shape `[samples[0].shape[0]] + samples[0].shape` (i.e. it must be an array of arrays that have the same shape as the `samples`.)

        Returns the total intra-cluster distance.
        """
        total_distance = 0
        for sample in samples:
            _, distance_squared = KMeans.__get_closest_centroid_index_and_distance(sample, centroids)
            total_distance += distance_squared

        return total_distance

    @staticmethod
    def __find_stable_centroids(samples, K, sensitivity, rng):
        """This is the core of the K-Means algorithm. This method calculates `K` stable centroids for the `samples` by iteratively assigning each sample to a cluster
        and recalculate the centroids. This stops when the largest movement of any centroid is lower than `sensitivity`.
        
        `samples` -- An ndarray of samples to cluster. Must be an ndarray.\n
        `K` -- The number of clusters to find.\n
        `sensitivity` -- The upper bound of allowed movement of centroids after an iteration.\n
        `rng` -- A NumPy RandomState instance.
        
        Returns an ndarray of `K` centroids of shape `[K] + samples[0].shape` (i.e. an array of arrays that have the same shape as the `samples`.)
        """
        # Determine K random centroids to start with
        centroids = samples[rng.choice(samples.shape[0], K, replace = False)].astype(np.float64)
        while True:
            new_centroid_counts, new_centroids = KMeans.__calculate_centroids(samples, centroids)

            # Now check if there are any empty clusters (centroids without assigned samples).
            # If so, reset those centroids to a random sample and recalculate all centroids. Repeat until there are no empty clusters.
            # In a bad case this will not stop. This means that K is way to high anyway. TODO: need to handle this case in a sensible way. Raise an error?
            while True:
                empty_cluster_indices = np.nonzero(new_centroid_counts == 0)[0]
                if empty_cluster_indices.shape[0] == 0:
                    break
                KMeans.__reset_empty_clusters(samples, empty_cluster_indices, new_centroids, rng)
                new_centroid_counts[empty_cluster_indices] = 1
                new_centroids[...] = np.diag(1.0 / new_centroid_counts) @ new_centroids
                new_centroid_counts[...], new_centroids[...] = KMeans.__calculate_centroids(samples, new_centroids)
                
            # Calculate new centroids by dividing the running totals by the number of samples assigned to each cluster.
            new_centroids[...] = np.diag(1.0 / new_centroid_counts) @ new_centroids
            centroid_movement = KMeans.__calculate_centroid_movement(centroids, new_centroids)
            centroids = new_centroids

            # If the largest movement is smaller than some small value, we can say that we converged to a solution.
            if centroid_movement.max() <= sensitivity ** 2:
                break
        
        return centroids

    def __init__(self, options, samples):
        """
        `options` -- A KMeansOptions instance.\n
        `samples` -- An ndarray with at least 2 dimensions. It contains the samples.
        """
        self.__options, self.__samples, self.__rng = options, samples, np.random.RandomState(options.seed)
        
    def determine_best_K(self):
        """Determine the best K to use from 1 up to and including `max_K`.
        
        The best K is found by using the elbow-method. It creates a plot of the total sum of squares of distances for each K
        and picks the best value for K by checking when the 3rd derivative passes through zero. When this happens it picks that K value - 1.
        However, the 3rd derivative can only calculated for K - 2 so the actual K picked will be K - 3.
        """
        max_K = self.__options.max_K

        K_dists = np.array([0.0] * 4, 'f')
        best_K = 0
        
        # If max_K is less then 4 we won't be able to calculate a 3nd derivative
        if max_K < 4:
            print('max_K < 4. Best K: {}'.format(max_K))
            return max_K

        for K in range(1, max_K + 1):
            _, distance = self.cluster(K)

            # Shift all elements one to the left (erasing the first one in the process)
            K_dists[:-1] = K_dists[1:]
            K_dists[-1] = distance

            print('{},{}'.format(K, distance)) # This is only here for demo purposes.

            if K >=4 and np.diff(K_dists, 3) >= 0:
                best_K = K-3
                break

        if best_K == 0:
            print('Could not find best K for max_K: {}'.format(max_K))
            return max_K
            
        return best_K

    def determine_cluster_ids(self, centroids):
        """Returns a 1 dimensional ndarray with cluster id's for each sample.

        The id's are an index into `centroids` for each sample indicating the centroid that is closest to it."""
        samples = self.__samples
        ids = np.empty(samples.shape[0], dtype = np.int)
        for i, sample in zip(range(samples.shape[0]), samples):
            ids[i], _ = KMeans.__get_closest_centroid_index_and_distance(sample, centroids)
        
        return ids

    def cluster(self, K):
        """Clusters the training samples into K clusters. The algorithm first calculates the best K. 

        `K` -- The number of clusters to find.
        
        Returns an ndarray with the centroids for each cluster. Also returns the total intra-cluster distance.
        """
        samples = self.__samples
        repeat = self.__options.repeat
        sensitivity = self.__options.sensitivity

        lowest_distance = float('inf')
        best_centroids = None
        for _ in range(repeat):
            centroids = KMeans.__find_stable_centroids(samples, K, sensitivity, self.__rng)
            distance = KMeans.__calculate_intra_distance(samples, centroids)
            if distance < lowest_distance:
                lowest_distance, best_centroids = distance, centroids

        return best_centroids, lowest_distance