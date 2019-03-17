// WingDesign.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "WingDesignPopMember.h"

#include "EvolutionaryAlgorithm/EvolutionaryAlgorithm.h"
#include "EvolutionaryAlgorithm/SelectionStrategy.h"

#include <iostream>
#include <random>
#include <algorithm>
#include <iomanip>

int main()
{
	int runs = 200;
	std::unordered_map<std::string, SelectionStrategy<WingDesignPopMember> *> strategies = {
		{ "Truncate0.2", new SelectionTruncate<WingDesignPopMember>{ 0.2 } },
		{ "Truncate0.4", new SelectionTruncate<WingDesignPopMember>{ 0.4 } },
		{ "Truncate0.6", new SelectionTruncate<WingDesignPopMember>{ 0.6 } },
		{ "Truncate0.8", new SelectionTruncate<WingDesignPopMember>{ 0.8 } },
		{ "Roulette", new SelectionRoulette<WingDesignPopMember>{} },
		{ "SUS", new SelectionSUS<WingDesignPopMember>{} },
		{ "Tournament2;1.0", new SelectionTournament<WingDesignPopMember>{ 2, { 1.0 } } },
		{ "Tournament2;0.8", new SelectionTournament<WingDesignPopMember>{ 2, { 0.8 } } },
		{ "Tournament2;0.6", new SelectionTournament<WingDesignPopMember>{ 2, { 0.6 } } },
		{ "Tournament3;1.0", new SelectionTournament<WingDesignPopMember>{ 3, { 1.0 } } },
		{ "Tournament3;0.8", new SelectionTournament<WingDesignPopMember>{ 3, { 0.8 } } },
		{ "Tournament3;0.6", new SelectionTournament<WingDesignPopMember>{ 3, { 0.6 } } },
		{ "Tournament4;1.0", new SelectionTournament<WingDesignPopMember>{ 4, { 1.0 } } },
		{ "Tournament4;0.8", new SelectionTournament<WingDesignPopMember>{ 4, { 0.8 } } },
		{ "Tournament4;0.6", new SelectionTournament<WingDesignPopMember>{ 4, { 0.6 } } },
	};
	
	for (const auto &[name, selection] : strategies)
	{
		std::cout << name << "\n";
		std::cout << std::right << std::setw(11) << "elites";
		for (int eliteSelecteesCount = 1; eliteSelecteesCount <= 10; eliteSelecteesCount++)
		{
			std::cout << std::right << std::setw(4) << eliteSelecteesCount << " ";
		}
		std::cout << "\n";

		for (int populationSize = 30; populationSize <= 150; populationSize += 20)
		{
			std::cout << "pops: " << std::right << std::setw(3) << populationSize << "  ";
			for (int eliteSelecteesCount = 1; eliteSelecteesCount <= 10; eliteSelecteesCount++)
			{
				std::vector<int> generations;
				int runnningTotalGens = 0;

				EvolutionaryAlgorithm<WingDesignPopMember> ea;
				ea.setPopulationSize(populationSize);
				ea.setEliteSelecteesCount(eliteSelecteesCount);
				ea.setSelectionStrategy(selection);
				ea.setStopCondition([&runnningTotalGens](int generation, const EvolutionaryAlgorithm<WingDesignPopMember>::Population & population)
				{
					if (population[0].getFitness() == 98938)
					{
						runnningTotalGens += generation;
						return true;
					}
					return false;
				});
				ea.setCallback([](int generation, const EvolutionaryAlgorithm<WingDesignPopMember>::Population & population)
				{
				});

				for (int i = 0; i < runs; i++)
				{
					ea.run();
				}

				std::cout << std::right << std::setw(4) << (runnningTotalGens / runs) << " ";
			}
			std::cout << "\n";
		}
		std::cout << "\n";
	}

	for (const auto &[name, selection] : strategies)
	{
		delete selection;
	}
}