#pragma once
#include "fwd_decl.h"

#include <vector>
#include <functional>

/// <summary>
/// The main evolutionary algorithm.
/// </summary>
/// <remarks>
/// Use the setters to configure the algorithm. Then call run() to run the algorithm
/// and receive back the evolved and sorted population in descending order.
/// </remarks>
/// <param name="TPopMember"> This is the type of the population members.
/// IPopMember is a convenience class that defines the necessary members that TPopMember must implement
/// to be able to be used by the evolutionary algorithm. However, it is not mandatory to derive from that class.</param>
template<class TPopMember>
class EvolutionaryAlgorithm
{
public:
	using Population = std::vector<TPopMember>;
	
private:
	static void sortDesc(Population & population);

public:
	/// <summary>
	/// Runs the algorithm. Call after having configured it using the setters.
	/// </summary>
	/// <returns> The evolved population, sorted on fitness in descending order.</returns>
	Population run();

	///////////////////
	// Settings
	/// <summary>
	/// Set the selection strategy to use. Several standard strategies are already implemented. This copies the shared_ptr.
	/// </summary>
	void setSelectionStrategy(const std::shared_ptr<SelectionStrategy<TPopMember>> & selectionStrategy);

	/// <summary>
	/// The the function to determine when to stop. The algorithm will stop if this returns true.
	/// </summary>
	void setStopCondition(const std::function<bool(int, const Population &)> & stopCondition);

	/// <summary>
	/// Called after each generation. Can be used for any additional work like printing things or tracking statistics.
	/// </summary>
	void setCallback(const std::function<void(int, const Population &)> & callback);

	void setPopulationSize(int populationSize);
	void setEliteSelecteesCount(int eliteSelecteesCount);

private:
	void evaluate();
	void recombinate(const Population & selectees);
	void mutate();

	Population population;

	///////////////////
	// Settings
	std::function<bool(int, const Population &)> stopCondition;
	std::function<void(int, const Population &)> callback;

	std::shared_ptr<SelectionStrategy<TPopMember>> selectionStrategy;

	int populationSize;
	int eliteSelecteesCount;
};

// Sorts the population on fitness in ascending order.
template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::sortDesc(Population & population)
{
	std::sort(population.begin(), population.end(),
		[](const TPopMember & left, const TPopMember & right)
		{
			return left.getFitness() > right.getFitness();
		}
	);
}

// Runs the algorithm. 
template<class TPopMember>
inline typename EvolutionaryAlgorithm<TPopMember>::Population EvolutionaryAlgorithm<TPopMember>::run()
{
	population = Population(populationSize);
	int generation = 0;
	
	evaluate();
	sortDesc(population);
	while (!stopCondition(generation, population))
	{
		Population selectees = selectionStrategy->select(population, (populationSize - eliteSelecteesCount) * 2);
		// Keep only the elites before recombination. recombinate() will append new children to the population after our elites.
		population.resize(eliteSelecteesCount);
		recombinate(selectees);
		mutate();
		evaluate();
		sortDesc(population);
		callback(generation, population);
		++generation;
	}

	return population;
}

///////////////////
// Settings
template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::setSelectionStrategy(const std::shared_ptr<SelectionStrategy<TPopMember>> & selectionStrategy)
{
	this->selectionStrategy = selectionStrategy;
}

template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::setStopCondition(const std::function<bool(int, const Population &)> & stopCondition)
{
	this->stopCondition = stopCondition;
}

template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::setCallback(const std::function<void(int, const Population &)> & callback)
{
	this->callback = callback;
}

template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::setPopulationSize(int populationSize)
{
	this->populationSize = populationSize;
}

template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::setEliteSelecteesCount(int eliteSelecteesCount)
{
	this->eliteSelecteesCount = eliteSelecteesCount;
}

///////////////////
// Private
template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::evaluate()
{
	for (TPopMember & member : population)
	{
		member.evaluate();
	}
}

// This function extends the population by recombinating the given selectees.
// selectees must contain parents of the new children.
// First, (populationSize - eliteSelecteesCount) children are generated and appended to the population.
// Then crossover() is called on each child n with parents selectees[n*2] and selectees[n*2+1].
template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::recombinate(const Population & selectees)
{
	for (int i = 0, end = static_cast<int>(selectees.size()); i < end; i += 2)
	{
		population.emplace_back().crossover(selectees[i], selectees[i + 1]);
	}
}

template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::mutate()
{
	// Make sure not to mutate the elite members.
	for (int i = eliteSelecteesCount; i < populationSize; ++i)
	{
		population[i].mutate();
	}
}
