#pragma once
#include "fwd_decl.h"

#include <vector>
#include <functional>

template<class TPopMember>
class EvolutionaryAlgorithm
{
public:
	using Population = std::vector<TPopMember>;

private:
	static void sortDesc(Population & population);

public:
	Population run();

	///////////////////
	// Settings
	void setSelectionStrategy(const SelectionStrategy<TPopMember> * selectionStrategy);

	void setStopCondition(const std::function<bool(int, const Population &)> & stopCondition);
	void setCallback(const std::function<void(int, Population &)> & callback);
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
	std::function<void(int, Population &)> callback;

	const SelectionStrategy<TPopMember> * selectionStrategy;

	int populationSize;
	int eliteSelecteesCount;
};

template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::sortDesc(Population & population)
{
	//Sort on fitness in ascending order
	std::sort(population.begin(), population.end(),
		[](const TPopMember & left, const TPopMember & right)
		{
			return left.getFitness() > right.getFitness();
		}
	);
}

template<class TPopMember>
inline typename EvolutionaryAlgorithm<TPopMember>::Population EvolutionaryAlgorithm<TPopMember>::run()
{
	population = Population(populationSize);
	int generation = 0;

	evaluate();
	sortDesc(population);
	while (!stopCondition(generation, population))
	{
		callback(generation, population);
		Population selectees = selectionStrategy->select(population, (populationSize - eliteSelecteesCount) * 2);
		// Keep only the elites before recombination
		population.resize(eliteSelecteesCount);
		recombinate(selectees);
		mutate();
		evaluate();
		sortDesc(population);
		++generation;
	}

	return population;
}

///////////////////
// Settings
template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::setSelectionStrategy(const SelectionStrategy<TPopMember> * selectionStrategy)
{
	this->selectionStrategy = selectionStrategy;
}

template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::setStopCondition(const std::function<bool(int, const Population &)> & stopCondition)
{
	this->stopCondition = stopCondition;
}

template<class TPopMember>
inline void EvolutionaryAlgorithm<TPopMember>::setCallback(const std::function<void(int, Population &)> & callback)
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
	for (int i = eliteSelecteesCount; i < populationSize; ++i)
	{
		population[i].mutate();
	}
}
