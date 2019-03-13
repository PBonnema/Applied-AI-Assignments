// WingDesign.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "WingDesignPopMember.h"

#include "EvolutionaryAlgorithm/EvolutionaryAlgorithm.h"
#include "EvolutionaryAlgorithm/SelectionStrategy.h"

#include <iostream>
#include <random>

int main()
{
	EvolutionaryAlgorithm<WingDesignPopMember> ea;
	//SelectionTruncate<WingDesignPopMember> selection{ 0.5 };
	SelectionRoulette<WingDesignPopMember> selection;
	ea.setPopulationSize(40);
	ea.setEliteSelecteesCount(2);
	ea.setSelectionStrategy(&selection);
	ea.setStopCondition([](int generation, const EvolutionaryAlgorithm<WingDesignPopMember>::Population & population)
	{
		return generation > 50;
	});
	ea.setCallback([](int generation, const EvolutionaryAlgorithm<WingDesignPopMember>::Population & population)
	{
		if (!(generation % 2))
		{
			std::cout << "Generation " << generation << ": " << population[0].getFitness() << "; " << population[0] << "\n";
		}
	});

	EvolutionaryAlgorithm<WingDesignPopMember>::Population population = ea.run();
	for (const WingDesignPopMember & member : population)
	{
		std::cout << member.getFitness() << ": " << member << "\n";
	}
}