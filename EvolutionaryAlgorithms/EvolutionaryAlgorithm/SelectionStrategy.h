#pragma once
#include "fwd_decl.h"
#include "Util.h"

#include <vector>
#include <random>
#include <algorithm>

///////////////////
// SelectionStrategy
template<class TPopMember>
class SelectionStrategy
{
public:
	virtual ~SelectionStrategy() = default;

	virtual std::vector<TPopMember> select(const std::vector<TPopMember> & population, int membersToSelect) const = 0;
};

///////////////////
// SelectionTournament
template<class TPopMember>
class SelectionTournament : public SelectionStrategy<TPopMember>
{
private:
	static const std::uniform_real_distribution<long double> uniformDblDist;
	static const std::uniform_int_distribution<std::size_t> uniformIntDist;

public:
	SelectionTournament(int tournamentSize, double p);
	virtual ~SelectionTournament() = default;

	virtual std::vector<TPopMember> select(const std::vector<TPopMember> & population, int membersToSelect) const override;

private:
	int calcWinningIndex() const;
	std::size_t randomInt(std::size_t min, std::size_t max) const;

	int tournamentSize;
	double p;
};

template<class TPopMember>
const std::uniform_real_distribution<long double> SelectionTournament<TPopMember>::uniformDblDist{ 0, 1 };
template<class TPopMember>
const std::uniform_int_distribution<std::size_t> SelectionTournament<TPopMember>::uniformIntDist;

template<class TPopMember>
inline SelectionTournament<TPopMember>::SelectionTournament(int tournamentSize, double p)
	: SelectionStrategy<TPopMember>{}
	, tournamentSize{ tournamentSize }
	, p{ p }
{
}

template<class TPopMember>
inline std::vector<TPopMember> SelectionTournament<TPopMember>::select(const std::vector<TPopMember> & population, int membersToSelect) const
{
	std::vector<TPopMember> selectees;
	selectees.reserve(membersToSelect);
	std::size_t popSize = population.size();
	for (int i = 0; i < membersToSelect; i++)
	{
		// Pick contenders at random
		std::vector<std::size_t> indices;
		indices.reserve(tournamentSize);
		for (int i = 0; i < tournamentSize; ++i)
		{
			std::size_t randomIndex = randomInt(0, popSize - 1);
			while (std::any_of(indices.cbegin(), indices.cend(), [&randomIndex](std::size_t index) { return index == randomIndex; }))
			{
				randomIndex = randomInt(0, popSize - 1);
			}
			indices.emplace_back(randomIndex);
		}

		int winningIndex = calcWinningIndex();
		std::nth_element(indices.begin(), indices.begin() + winningIndex, indices.end());
		selectees.push_back(population[indices[winningIndex]]);
	}
	return selectees;
}

///////////////////
// Private
template<class TPopMember>
inline int SelectionTournament<TPopMember>::calcWinningIndex() const
{
	double number = uniformDblDist(RANDOM_ENG);
	double remainingFraction = 1.0 - p;

	int index = 0;
	while (remainingFraction > number && index < tournamentSize - 1)
	{
		++index;
		remainingFraction *= 1.0 - p;
	}

	return index;
}

template<class TPopMember>
inline std::size_t SelectionTournament<TPopMember>::randomInt(std::size_t min, std::size_t max) const
{
	return uniformIntDist(RANDOM_ENG, decltype(uniformIntDist)::param_type{ min, max });
}

///////////////////
// SelectionTruncate
template<class TPopMember>
class SelectionTruncate : public SelectionStrategy<TPopMember>
{
public:
	SelectionTruncate(double fraction);
	virtual ~SelectionTruncate() = default;

	virtual std::vector<TPopMember> select(const std::vector<TPopMember> & population, int membersToSelect) const override;

private:
	double fraction;
};

template<class TPopMember>
inline SelectionTruncate<TPopMember>::SelectionTruncate(double fraction)
	: SelectionStrategy<TPopMember>{}
	, fraction{ fraction }
{
}

template<class TPopMember>
inline std::vector<TPopMember> SelectionTruncate<TPopMember>::select(const std::vector<TPopMember> & population, int membersToSelect) const
{
	std::vector<TPopMember> selectees;
	selectees.reserve(membersToSelect);
	std::size_t maxIndex = std::max(static_cast<std::size_t>(population.size() * fraction), 1ull);
	for (int i = 0; i < membersToSelect; i++)
	{
		selectees.push_back(population[i % maxIndex]);
	}
	std::shuffle(selectees.begin(), selectees.end(), RANDOM_ENG);
	return selectees;
}

///////////////////
// SelectionRoulette
template<class TPopMember>
class SelectionRoulette : public SelectionStrategy<TPopMember>
{
private:
	static const std::uniform_real_distribution<long double> uniformDblDist;

public:
	virtual ~SelectionRoulette() = default;

	virtual std::vector<TPopMember> select(const std::vector<TPopMember> & population, int membersToSelect) const override;
};

template<class TPopMember>
const std::uniform_real_distribution<long double> SelectionRoulette<TPopMember>::uniformDblDist{ 0, 1 };

template<class TPopMember>
inline std::vector<TPopMember> SelectionRoulette<TPopMember>::select(const std::vector<TPopMember> & population, int membersToSelect) const
{
	// Calculate the cumulative fitness values and put them in a vector
	std::vector<double> cumulativeNormalizedFitness(population.size());
	// Use lowestFitness for normalization if it is negative.
	// Multiply it by 2 so that the worst member won't end up with a fitness 0 zero, giving it a chance to be selected.
	double lowestFitness = std::min(population.back().getFitness() * 2, 0.0);
	std::transform_inclusive_scan(population.cbegin(), population.cend(), cumulativeNormalizedFitness.begin(),
		std::plus<>(),
		[lowestFitness](const TPopMember & member) { return member.getFitness() - lowestFitness; });

	// Now do the actual roulette to select enough members
	double wheelSize = cumulativeNormalizedFitness.back();
	std::vector<TPopMember> selectees;
	selectees.reserve(membersToSelect);
	for (int i = 0; i < membersToSelect; i++)
	{
		double pickedFitness = uniformDblDist(RANDOM_ENG) * wheelSize;
		auto selecteeIt = std::lower_bound(cumulativeNormalizedFitness.cbegin(), cumulativeNormalizedFitness.cend(), pickedFitness);
		std::size_t indexSelectee = selecteeIt - cumulativeNormalizedFitness.cbegin();
		selectees.push_back(population[indexSelectee]);
	}

	return selectees;
}

///////////////////
// SelectionSUS
template<class TPopMember>
class SelectionSUS : public SelectionStrategy<TPopMember>
{
private:
	static const std::uniform_real_distribution<long double> uniformDblDist;

public:
	virtual ~SelectionSUS() = default;

	virtual std::vector<TPopMember> select(const std::vector<TPopMember> & population, int membersToSelect) const override;
};

template<class TPopMember>
const std::uniform_real_distribution<long double> SelectionSUS<TPopMember>::uniformDblDist{ 0, 1 };

template<class TPopMember>
inline std::vector<TPopMember> SelectionSUS<TPopMember>::select(const std::vector<TPopMember> & population, int membersToSelect) const
{
	// Calculate the cumulative fitness values and put them in a vector
	std::vector<double> cumulativeNormalizedFitness(population.size());
	// Use lowestFitness for normalization if it is negative.
	// Multiply it by 2 so that the worst member won't end up with a fitness 0 zero, giving it a chance to be selected.
	double lowestFitness = std::min(population.back().getFitness() * 2, 0.0);
	std::transform_inclusive_scan(population.cbegin(), population.cend(), cumulativeNormalizedFitness.begin(),
		std::plus<>(),
		[lowestFitness](const TPopMember & member) { return member.getFitness() - lowestFitness; });

	// Now do the actual roulette to select enough members
	double averageFitnessSeparation = cumulativeNormalizedFitness.back() / membersToSelect;
	double currentPoint = uniformDblDist(RANDOM_ENG) * averageFitnessSeparation;
	std::vector<TPopMember> selectees;
	selectees.reserve(membersToSelect);
	for (int i = 0; i < membersToSelect; i++)
	{
		auto selecteeIt = std::lower_bound(cumulativeNormalizedFitness.cbegin(), cumulativeNormalizedFitness.cend(), currentPoint);
		std::size_t indexSelectee = selecteeIt - cumulativeNormalizedFitness.cbegin();
		selectees.push_back(population[indexSelectee]);
		currentPoint += averageFitnessSeparation;
	}

	return selectees;
}