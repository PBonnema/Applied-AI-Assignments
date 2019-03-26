# Applied-AI-Assignments
This repo contains the assigments for my Applied AI course.

To run any assignment, run the main.py file from within the respective folder.
Most documentation can be found in the code files themselves.

## K-Means
./K-Means contains an Excel sheet with a screeplot that shows that K has to be either 2 or 3.

The code also contains a simple algorithm to find the best K. It is where the third derivative of the screeplot is >= 0.
By that logic, it would find a K of 3 for the particular screeplot in the Excel sheet.

The actual K found by the code heavily depends on the seed (which can be specified in main.py).

## NN Programming
The main.py file for this assignment also contains the assignments 4.3 A through 4.3 C from the reader.
The assignment 4.3 D can be found near the bottom.

I have tried many different topologies to build a classifier for the Iris dataset.
The first consideration is that (according to the website of the dataset) the data contains 3 classes, 2 of which aren't linearly seperable. This means that at least 1 hidden layer is needed with probably more than 2 neurons.

What also can be said is that the data is not complex enough to warrant a large number of hidden layers (like >= 5).
My experiments confirmed this. I never managed to bring the mean squared error to below 0.01 and this was done with a single hidden layer (this result was a statistical fluke however).

My final topology contains 1 hidden layer with 4 sigmoid neurons and the 4 mandatory inputs and 3 outputs of course.
These are all the settings:

```
validation_percentage = 0.6 # The percentage of the data that will be set aside as a validation set.
minimum_certainty = 0.8 # The minimum certainty of outputs before it will be rounded to either 0 or 1. Otherwise, it will be left as is and will thus be incorrect.
learning_rate = 0.12
epochs = 500
weight_lower_bound = -1 # Used during weight randomization.
weight_upper_bound = 1 # Used during weight randomization.
```

Its performance differs between seeds but fluctuates between a mean squared error (MSE) of ~0.8 and ~0.2. The percentage of correctly classified samples fluctuates between 90% and 98% with occasional dips to ~85%.

The learning rate was empirically found by plotting the evolution of the MSE during training. 0.12 is about as high it can be before it starts overshooting minima (clearly visible in the plot).

Other topologies were also tried. Topologies with more layers had comparable performances so that didn't pay off. A larger number of neurons in the layer also didn't pay off.

Topologies containing leaky ReLU (LReLU) neurons did as well as sigmoid neurons but required slightly more neurons (atleast 6) though the output neurons still had to be sigmoid neurons since the required outputs are the extremes (0 and 1) and LReLU's have a hard time converging on those.

I've also tried tanh neurons. These topologies required far more neurons and a lower learning rate. A topology with 2 hidden layers and 6 neurons each performed well but still did not any better. These also needed sigmoid neurons as outputs for the same reasons.

## NN Algebraic representation
I started by looking up a tutorial about Tensorflow and Keras. By chance, this tutorial also implemented an NN for MNIST. The suggested implementation can reach a validation accuracy of ~97% within 10 epochs. However, the used network is rather large with 2 hidden layers of 128 neurons each.

I measured the accuracy of the models and speed of convergence using tensorboard. I've committed a series of TensorBoard log files along with the code. The way that I interpret them is by looking at the epoch_val_acc and epoch_val_loss graphs. Higher or lower, respectively, is better. Often, the epoch_val_loss starts growing again after a couple of epochs. This indicates overfitting. The epoch at which this starts to happen is the best number of epochs.

I started out by experimenting with batch size. Changing this around made a small impact on accuracy but a huge impact on execution times. Smaller batches were slower. I tried up to a batch size of 70 and I sticked with it. The val_loss graph show that the lowest point for most lines lays around 3 or 4 epochs.

Next, I have tried improving the results with 4 different kinds of activation functions: sigmoid, relu, lrelu and tanh. Nothing resulted in better results compared to the original relu. The only real difference was how fast the NN converges but relu also converges the fastest after approximately 3 echos.

Lastly, I tried different kinds of topologies: 

|hidden layer sizes|val_acc|ns/sample|
|-|-|-|
|(64, 64)|0.9687|50|
|(256, 256)|0.9774|100|
|(128, 64, 32)|0.9717|70|
|(30, 20, 10)|0.9589|52|
|(28*28, 20)|0.9754|280|.

Nothing resulted in better results in terms of validation accuracy but the execution times were influenced heavily depending on the number of neurons, weights and number of layers. The network of (30, 20, 10) was the smallest network and ran almost the fastest but scored pretty well. However, the much larger (64, 64) network scored a bit higher but had the same execution times despite having twice the number of weights. This shows that the number of layers plays a big role in run times for smaller networks but that larger hidden layers are needed to maintain accuracy. In larger networks the number of weights start to dominate.

I would choose (128, 64, 32) to strike a balance between performance and accuracy.

## Evolutionary algorithms
I choose the wing design problem to solve. To find a good set of parameters for the algorithm I semi-bruteforced it. I ran a large number of parameter combinations and measured the speed of convergence over 200 runs. The convergence is defined as the number of generations it takes to reach the perfect fitness of 98938 for at least 1 population member.
The results can be found under .\EvolutionaryAlgorithm in the text files called meanTablex.txt.

Each suchs text file contains a table of convergence means (over 200 runs) for each selection strategy that was implemented.
The tables show the averages for each combination of elite member count and population size for that strategy that was tried.
Every time that a set of parameters was run I interpreted the results and tried other parameters (producing the next text file).

The selection strategies that I implemented are: tournament, truncate, fitness proportional roulette, and stochastic universal sampling (SUS).
For each table the name of the strategy and its own parameters are printed.
- Tournament is printed as Tournament<tournament size>\;\<p variable>
- Truncate is printed as Truncate\<fraction>.
- SUS and fitness proportional roulette are printed as SUS and Roulette.
  
In meanTables1 it became clear that truncate performed much better compared to Roulette and SUS. It also became clear that a population size under 40 doesn't perform well. It also shows that Roulette and SUS benefit from more elite members while Truncate slightly suffers.

In MeanTables2, 0 elite members was tried. Did didn't work out well. Roulette performed very bad.

In MeanTables3, I added tournament selection as it was finished by then. I also upped the range of population size and elite member count. It became clear that a very high selection pressure works best in combination with a large population size. The number of elites didn't matter that much anymore for truncate and tournament.

In MeanTables4 I left out roulette and SUS and increased the selection pressure. The results became much better.
I did the same in MeanTables5.

The best performing parameter combination I saw is:
- Truncate selection with a fraction parameter of 0.01
- Population size of 200
- Any number of elite members

This converges within 5 generations on average.

Tournament14;1.0 with a pop size of 200 and any number of elites converges within 6 generations on average.

In the end I didn't bother to test different bitflip chances as well sicne the performance was this good already. For all tests I used a flip chance of 0.8 / 24 (the number of bits in the bitstring).
