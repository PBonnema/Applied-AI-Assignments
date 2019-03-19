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

	/// <summary>
	/// Must return membersToSelect selectees from the population. These will act as parents during recombination.
	/// The returned vector may contain the same selectee twice.
	/// </summary>
	/// <returns> The selectees</returns>
	virtual std::vector<TPopMember> select(const std::vector<TPopMember> & population, int membersToSelect) const = 0;
};

///////////////////
// SelectionTournament
/// <summary>
/// Implements the tournament selection algorithm. The selection pressure can be tuned with the tournament size and the p variable.
/// p is the chance that the winner is selected. If it's not selected, then the 2nd place as a p chance to be selected etc.
/// </summary>
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
		// Pick contenders at random and store their index.
		std::vector<std::size_t> indices;
		indices.reserve(tournamentSize);
		for (int i = 0; i < tournamentSize; ++i)
		{
			std::size_t randomIndex = randomInt(0, popSize - 1);
			// Keep picking a new index if the current index has already been choosen.
			while (std::any_of(indices.cbegin(), indices.cend(), [&randomIndex](std::size_t index) { return index == randomIndex; }))
			{
				randomIndex = randomInt(0, popSize - 1);
			}
			indices.emplace_back(randomIndex);
		}

		// Now calculate which place in the tournament result (1th place, 2nd place etc) will be selected.
		int winningIndex = calcWinningIndex();
		// And figure out the index of the selectee.
		std::nth_element(indices.begin(), indices.begin() + winningIndex, indices.end());
		selectees.push_back(population[indices[winningIndex]]);
	}
	return selectees;
}

///////////////////
// Private

// Calculates which place in the tournament result (1th place, 2nd place etc) will be selected.
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

// Simply chop off the lower fraction part of the population and then repeat the upper portion as many times as needed to select enough members.
template<class TPopMember>
inline std::vector<TPopMember> SelectionTruncate<TPopMember>::select(const std::vector<TPopMember> & population, int membersToSelect) const
{
	std::vector<TPopMember> selectees;
	selectees.reserve(membersToSelect);
	// This is the highest index that we can pick from. Make sure there is always at least 1 possible index.
	std::size_t maxIndex = std::max(static_cast<std::size_t>(population.size() * fraction), 1ull);

	// Append the allowed portion of the population to selectees as many times as needed. Don't bother to randomly select members.
	for (int i = 0; i < membersToSelect; i++)
	{
		selectees.push_back(population[i % maxIndex]);
	}

	// Now shuffle the selectees as they were kept in order of fitness by the code above.
	// This is needed since their order also specifies which selectee will mate with whom during recombination and we want this to vary.
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