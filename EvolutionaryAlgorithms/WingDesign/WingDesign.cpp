// WingDesign.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "WingDesignPopMember.h"

#include "EvolutionaryAlgorithm/EvolutionaryAlgorithm.h"
#include "EvolutionaryAlgorithm/SelectionStrategy.h"

#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <memory>

int main()
{
	int runs = 200;
	double bestFitness = 98938; // Only used to determine the rate of convergence.
	std::unordered_map<std::string, std::shared_ptr<SelectionStrategy<WingDesignPopMember>>> strategies = {
		{ "Truncate0.01", std::make_shared<SelectionTruncate<WingDesignPopMember>>(0.01) },
		{ "Truncate0.02", std::make_shared<SelectionTruncate<WingDesignPopMember>>(0.02) },
		{ "Truncate0.05", std::make_shared<SelectionTruncate<WingDesignPopMember>>(0.05) },
		{ "Tournament10;1.0", std::make_shared<SelectionTournament<WingDesignPopMember>>(10, 1.0) },
		{ "Tournament10;0.8", std::make_shared<SelectionTournament<WingDesignPopMember>>(10, 0.8) },
		{ "Tournament12;1.0", std::make_shared<SelectionTournament<WingDesignPopMember>>(12, 1.0) },
		{ "Tournament12;0.8", std::make_shared<SelectionTournament<WingDesignPopMember>>(12, 0.8) },
		{ "Tournament14;1.0", std::make_shared<SelectionTournament<WingDesignPopMember>>(14, 1.0) },
		{ "Tournament14;0.8", std::make_shared<SelectionTournament<WingDesignPopMember>>(14, 0.8) },
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

		for (int populationSize = 100; populationSize <= 200; populationSize += 20)
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
				// Use this stop condition to determine the rate of convergence. It uses knowledge of the problem (the best fitness).
				ea.setStopCondition([&bestFitness, &runnningTotalGens](int generation, const EvolutionaryAlgorithm<WingDesignPopMember>::Population & population)
				{
					if (population[0].getFitness() == bestFitness)
					{
						runnningTotalGens += generation;
						return true;
					}
					return false;
				});
				// Use a stop condition like this if you simply want to run the algorithm once.
				//ea.setStopCondition([](int generation, const EvolutionaryAlgorithm<WingDesignPopMember>::Population & population)
				//{
				//	return generation > 50; // Or do something complex with population.
				//});
				ea.setCallback([](int generation, const EvolutionaryAlgorithm<WingDesignPopMember>::Population & population)
				{
					// Can do anything like drawing some simulator, collecting statistics etc.
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
}