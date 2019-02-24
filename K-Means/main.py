from importData import import_training_data, import_validation_data, import_unlabeled
from KMeans import KMeans

import numpy as np

# Import the training data, validation data, and the unlabeled data
training_labels, training_samples = import_training_data('..\\dataset1.csv')
labeled_samples = import_unlabeled('..\\days.csv')

K_means_option = KMeans.KMeansOptions(15, 0.01, 10, None)
k_means = KMeans(K_means_option, training_samples)
best_K = k_means.determine_best_K()
print('Best K: {}\nYou can copy paste the printed distances above for each K into excel to visualise the scree plot.'.format(best_K))
centroids, _ = k_means.cluster(best_K)
cluster_ids = k_means.determine_cluster_ids(centroids)

# Op basis van de screeplot zou ik zeggen dat de beste K 2 of 3 moet zijn. Het is niet helemaal duidelijk. Het algoritme vindt vaak andere K's.

print('centroids:\n{}\ncluster_ids:\n{}'.format(centroids, cluster_ids))

# Now figure out what labels are most common in each cluster
unique_training_labels = np.unique(training_labels)
cluster_label_counts = np.zeros((centroids.shape[0], unique_training_labels.shape[0]), dtype='i')

for cluster_id, correct_label in zip(cluster_ids, training_labels):
    cluster_label_counts[cluster_id][np.nonzero(unique_training_labels == correct_label)] += 1

labels_for_cluster_ids = unique_training_labels[np.argmax(cluster_label_counts, axis=1)]

# Calculate the percentage of samples who's original label equals the label of its cluster
correct_count = 0
for cluster_id, correct_label in zip(cluster_ids, training_labels):
    if(labels_for_cluster_ids[cluster_id] == correct_label):
        correct_count += 1

print('Correctly labeled: {} (is {}%)\nLabels for cluster ids: {}'.format(correct_count, 100 * correct_count / training_labels.shape[0], labels_for_cluster_ids))